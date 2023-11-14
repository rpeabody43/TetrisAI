#include <gtest/gtest.h>
#include <cmath>

#include "../src/ai/genetic/Agent.h"

/* This test verifies that Agent.GenInput() moves the piece correctly */
TEST(TestGenInput, BasicAssertions)
{
    std::default_random_engine randomEngine (0);
    Board board (250, randomEngine);
    // weights shouldn't matter as long as they're consistent
    Weights weights = {-20.0, -10.0, 50.0, -1.0, -20.0, -10.0};
    Agent agent (true, weights);

    int movesPlayed = 0;
    uint8_t previousPiece = 0;
    Input input = {};
    board.Update(input, 1);

    Move workingMove = {};

    int8_t rotDelta;
    int8_t colDelta;

    while (movesPlayed < 10 && !board.GameOver())
    {
        if (board.GetFallingPiece() != previousPiece)
        {
            if (previousPiece > 0)
                movesPlayed++;
            previousPiece = board.GetFallingPiece();
            rotDelta = 3;
            colDelta = 10;
            workingMove = BestMove(&board, weights);
        }

        int8_t newRotDelta = workingMove.rotation - board.GetFallingPieceRot();
        int8_t offset = TetrominoData::GetPieceBounds(board.GetFallingPiece(), board.GetFallingPieceRot()).leftBound;
        uint8_t targetCol = Board::Col(workingMove.position - offset);
        uint8_t currentCol = Board::Col(board.GetFallingPieceAnchor() - offset);
        int8_t newColDelta = targetCol - currentCol;

        // Assert that the piece is closer to the "end" position than it was before
        if (colDelta == 0)
        {
            ASSERT_EQ(newColDelta, 0);
        }
        else if (colDelta > 0)
        {
            ASSERT_LT(newColDelta, colDelta);
        }
        else
        {
            ASSERT_GT(newColDelta, colDelta);
        }
        ASSERT_LE(abs(newRotDelta), abs(rotDelta));
        colDelta = newColDelta;
        rotDelta = newRotDelta;

        // Update board for next cycle
        input = agent.GenInput(&board);
        board.Update(input, 1);
    }
}