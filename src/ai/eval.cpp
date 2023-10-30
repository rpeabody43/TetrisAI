#include <cmath>
#include <vector>
#include <cfloat>

#include "eval.h"
#include "../game/Board.h"
#include "../game/tetrominoes.hpp"

struct BoardAnalysis
{
    int holesCount;         // Open squares with filled squares above
    int aggregateHeight;    // The total number of filled squares
    int completeLines;      // Amount of lines to be cleared
    double heightStdDev;    // Flatter board = better
};

double GetHeightStdDev (const int highestPoints[Board::WIDTH])
{
    int sum = 0;
    for (int i = 0; i < Board::WIDTH; i++)
    {
        sum += highestPoints[i];
    }

    double avg = (double) sum / Board::WIDTH;
    double standardDev = 0;
    for (int i = 0; i < Board::WIDTH; i++)
    {
        double dev = highestPoints[i] - avg;
        standardDev += dev * dev;
    }
    return std::sqrt(standardDev) / Board::WIDTH;
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
            if (squareFilled && columnHeights[x] == 0)
                columnHeights[x] = y;

            if (squareFilled)
            {
                vals.aggregateHeight++;
            }
            else
            {
                // If this isn't the first square in the column and isn't filled
                if (columnHeights[x] > 0)
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

std::vector<Move> GenerateMoves (Board* currentBoard, int currentPiece, int heldPiece)
{
    std::vector<Move> moveList;


    return moveList;
}

Move BestMove (Board* currentBoard, int currentPiece, int heldPiece)
{
    struct Weights
    {
        double holesCount;
        double aggregateHeight;
        double completeLines;
        double heightStdDev;
    };
    Weights weights = {-1.0, -1.0, 50.0, -1.0};

    std::vector<Move> moveList = GenerateMoves(currentBoard, currentPiece, heldPiece);
    Move bestMove = {};
    double bestScore = -DBL_MAX;

    for (auto& move: moveList)
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
        }
    }

    return bestMove;
}
