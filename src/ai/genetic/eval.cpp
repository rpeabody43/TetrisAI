#include <cmath>
#include <vector>
#include <cfloat>

#include "eval.h"
#include "game/Board.h"
#include "game/tetrominoes.hpp"

struct BoardAnalysis
{
    uint8_t holesCount;         // Open squares with filled squares above
    uint16_t aggregateHeight;   // The total number of filled squares
    uint8_t completeLines;      // Amount of lines to be cleared
    double heightStdDev;        // Flatter board = better
    uint8_t highestPoint;       // Highest point reached
    uint8_t blocksOverHoles;    // How many blocks are above holes in the board
};

/**
 * @param highestPoints An array of the highest points in each column.
 * @return The standard deviation of heights.
 */
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

/**
 * Runs each of the heuristics on the current board with a hypothetical move.
 * @param currentBoard The current board state.
 * This method does not modify the Board object.
 * @param pieceAnchor Where the proposed move would end.
 * @param piece Which piece the move is with.
 * @param pieceRot The rotation of the piece after the move.
 * @return A BoardAnalysis object with various heuristics
 */
BoardAnalysis AnalyzeBoard (Board* currentBoard, int pieceAnchor, int piece, int pieceRot)
{
    int pieceSquares[4] = {};
    int squareIdx = 0;
    for (int i = 0; i < 4; i++)
    {
        pieceSquares[i] = pieceAnchor + TetrominoData::GetPieceMap(piece, pieceRot, i);
    }

    BoardAnalysis vals = {};
    vals.highestPoint = currentBoard->GetHighestRow();
    if (vals.highestPoint > Board::Row(pieceAnchor))
        vals.highestPoint = Board::Row(pieceAnchor);


    int columnHeights[Board::WIDTH] = {};
    int columnHoles[Board::WIDTH] = {};

    // Fiill all heights with the "lowest" square
    std::fill_n(columnHeights, Board::WIDTH, Board::HEIGHT);

    for (int y = vals.highestPoint; y < Board::HEIGHT; y++)
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
                {
                    columnHoles[x]++;
                    vals.blocksOverHoles += (y-columnHeights[x]) - columnHoles[x];
                }
                lineComplete = false;
            }
        }
        if (lineComplete)
            vals.completeLines++;
    }

    for (int holes : columnHoles)
    {
        vals.holesCount += holes;
    }
    vals.heightStdDev = GetHeightStdDev(columnHeights);
    vals.highestPoint = Board::HEIGHT-vals.highestPoint; // Make higher number -> higher on board

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

/**
 * Gets all possible "hard drop" moves on the current board
 * @param currentBoard The current board state.
 * This method does not modify the Board object.
 * @param currentPiece The current falling piece.
 * @param heldPiece The held piece or the next piece up if no piece is held.
 * @return A vec of Move objects, each with an ending anchor, rotation, and if it includes a hold
 */
std::vector<Move> GenerateMoves (Board* currentBoard, uint8_t currentPiece, uint8_t heldPiece)
{
    std::vector<Move> moveList;
    for (int8_t piece : {currentPiece, heldPiece})
    {
        uint8_t numRot;
        switch (piece)
        {
            case O_PIECE:
                numRot = 1;
                break;
            case S_PIECE:
            case Z_PIECE:
            case I_PIECE:
                numRot = 2;
                break;
            default:
                numRot = 4;
                break;
        }
        for (int rot = 0; rot < numRot; rot++)
        {
            TetrominoData::Bounds pieceBounds = TetrominoData::GetPieceBounds(piece, rot);
            for (uint8_t startPos = pieceBounds.leftBound + Board::BUFFER_SQUARES; startPos <= pieceBounds.rightBound +  Board::BUFFER_SQUARES; startPos++)
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

    for (Move& move : moveList)
    {
        int piece = move.hold ? heldPiece : currentPiece;
        BoardAnalysis analysis = AnalyzeBoard(currentBoard, move.position, piece, move.rotation);

        double score = analysis.holesCount * weights.holesCount +
                       analysis.aggregateHeight * weights.aggregateHeight +
                       analysis.completeLines * weights.completeLines +
                       analysis.heightStdDev * weights.heightStdDev +
                       analysis.highestPoint * weights.highestPoint +
                       analysis.blocksOverHoles * weights.blocksOverHoles;
        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}
