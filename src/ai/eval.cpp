#include <cmath>
#include <vector>
#include <cfloat>

#include "eval.h"
#include "../game/Board.h"
#include "../game/tetrominoes.hpp"

struct BoardAnalysis
{
    uint8_t holesCount;         // Open squares with filled squares above
    uint16_t aggregateHeight;   // The total number of filled squares
    uint8_t completeLines;      // Amount of lines to be cleared
    double heightStdDev;        // Flatter board = better
};

double GetHeightStdDev (const int highestPoints[Board::WIDTH])
{
    int sum = 0, i;
    double avg, dev, standardDev = 0;
    for (i = 0; i < Board::WIDTH; i++)
    {
        sum += highestPoints[i];
    }

    avg = (double) sum / Board::WIDTH;
    for (i = 0; i < Board::WIDTH; i++)
    {
        dev = highestPoints[i] - avg;
        standardDev += dev * dev;
    }
    return std::sqrt(standardDev / Board::WIDTH);
}

BoardAnalysis AnalyzeBoard (Board* currentBoard, int pieceAnchor, int piece, int pieceRot)
{
    int pieceSquares[4] = {};
    int squareIdx = 0;
    for (int i = 0; i < 4; i++)
    {
        pieceSquares[i] = pieceAnchor + TetrominoData::GetPieceMap(piece, pieceRot, i);
    }

    int highestPoint = currentBoard->GetHighestRow();
    if (highestPoint > Board::Row(pieceAnchor))
        highestPoint = Board::Row(pieceAnchor);

    BoardAnalysis vals = {};

    int columnHeights[Board::WIDTH] = {};
    // Fiill all heights with the "lowest" square
    std::fill_n(columnHeights, Board::WIDTH, Board::HEIGHT);

    for (int y = highestPoint; y < Board::HEIGHT; y++)
    {
        bool lineComplete = true;
        for (int x = 0; x < Board::WIDTH; x++)
        {
            // Consider the square "filled" if there's something there or the current piece fills it
            bool pieceAtIdx = squareIdx < 4 && pieceSquares[squareIdx] == Board::ConvertIdx(x, y);
            if (pieceAtIdx)
                squareIdx++;

            bool squareFilled = pieceAtIdx || currentBoard->GetSquare(x, y) > 0;

            // Going from top down, so first filled square is the highest
            if (squareFilled && columnHeights[x] == Board::HEIGHT)
                columnHeights[x] = y;

            if (squareFilled)
            {
                vals.aggregateHeight++;
            }
            else
            {
                // If this isn't the first square in the column and isn't filled
                if (columnHeights[x] < 24)
                    vals.holesCount++;
                lineComplete = false;
            }
        }
        if (lineComplete)
            vals.completeLines++;
    }
    vals.heightStdDev = GetHeightStdDev(columnHeights);

    return vals;
}

bool ValidStart (Board* currentBoard, uint8_t piece, uint16_t anchor, uint8_t rot)
{
    for (int i = 0; i < 4; i++)
    {
        uint8_t squareIndex = anchor + TetrominoData::GetPieceMap(piece, rot, i);
        if (currentBoard->GetSquare(squareIndex) > 0)
        {
            return false;
        }
    }
    return true;
}

std::vector<Move> GenerateMoves (Board* currentBoard, uint8_t currentPiece, uint8_t heldPiece)
{
    std::vector<Move> moveList;
    for (int8_t piece : {currentPiece, heldPiece})
    {
        uint8_t numRot;
        switch (piece)
        {
            case TetrominoData::O:
                numRot = 1;
                break;
            case TetrominoData::S:
            case TetrominoData::Z:
            case TetrominoData::I:
                numRot = 2;
                break;
            default:
                numRot = 4;
                break;
        }
        for (int rot = 0; rot < numRot; rot++)
        {
            TetrominoData::Bounds pieceBounds = TetrominoData::GetPieceBounds(piece, rot);
            for (uint8_t startPos = pieceBounds.leftBound + Board::BUFFER_HEIGHT*10; startPos <= pieceBounds.rightBound + Board::BUFFER_HEIGHT*10; startPos++)
            {
                if (!ValidStart(currentBoard, piece, startPos, rot))
                    continue;
                uint16_t endingPos = currentBoard->GetGhost(piece, startPos, rot);
                moveList.push_back({
                    .position = endingPos,
                    .rotation = rot,
                    .hold = piece == heldPiece
                });
            }
        }
        if (currentPiece == heldPiece)
            break;
    }

    return moveList;
}

Move BestMove (Board* currentBoard, Weights& weights)
{
    uint8_t currentPiece = currentBoard->GetFallingPiece();
    uint8_t heldPiece = currentBoard->GetHeldPiece();
    // Treat the next piece up as the held piece if nothing is held
    if (heldPiece == 0)
        heldPiece = currentBoard->NthPiece(0);

    std::vector<Move> moveList = GenerateMoves(currentBoard, currentPiece, heldPiece);
    Move bestMove = {};
    double bestScore = -DBL_MAX;
    BoardAnalysis bestAnal {};

    for (Move& move : moveList)
    {
        int piece = move.hold ? heldPiece : currentPiece;
        BoardAnalysis analysis = AnalyzeBoard(currentBoard, move.position, piece, move.rotation);

        double score = analysis.holesCount * weights.holesCount +
                       analysis.aggregateHeight * weights.aggregateHeight +
                       analysis.completeLines * weights.completeLines +
                       analysis.heightStdDev * weights.heightStdDev;
        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
            bestAnal = analysis;
        }
    }

    return bestMove;
}
