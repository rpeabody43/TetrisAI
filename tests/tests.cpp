#include <gtest/gtest.h>
#include <cmath>

#include "../src/ai/genetic/Agent.hpp"

/* This test verifies that Agent.gen_input() moves the piece correctly */
TEST(Testgen_input, BasicAssertions) {
    std::default_random_engine random_engine (0);
    Board board (250, random_engine);
    // weights shouldn't matter as long as they're consistent
    Weights weights = {-20.0, -10.0, 50.0, -1.0, -20.0, -10.0};
    Agent agent (true, weights);

    int moves_played = 0;
    uint8_t previous_piece = 0;
    Input input = {};
    board.update(input, 1);

    Move working_move = {};

    int8_t rot_delta;
    int8_t col_delta;

    while (moves_played < 10 && !board.game_over())
    {
        if (board.get_falling_piece() != previous_piece)
        {
            if (previous_piece > 0)
                moves_played++;
            previous_piece = board.get_falling_piece();
            rot_delta = 3;
            col_delta = 10;
            working_move = best_move(&board, weights);
        }

        int8_t new_rot_delta = working_move.rotation - board.get_falling_piece_rot();
        int8_t offset = tetromino_data::get_piece_bounds(board.get_falling_piece(), board.get_falling_piece_rot()).left_bound;
        uint8_t target_col = Board::col(working_move.position - offset);
        uint8_t current_col = Board::col(board.get_falling_piece_anchor() - offset);
        int8_t new_col_delta = target_col - current_col;

        // Assert that the piece is closer to the "end" position than it was before
        if (col_delta == 0)
        {
            ASSERT_EQ(new_col_delta, 0);
        }
        else if (col_delta > 0)
        {
            ASSERT_LT(new_col_delta, col_delta);
        }
        else
        {
            ASSERT_GT(new_col_delta, col_delta);
        }
        ASSERT_LE(abs(new_rot_delta), abs(rot_delta));
        col_delta = new_col_delta;
        rot_delta = new_rot_delta;

        // update board for next cycle
        input = agent.gen_input(&board);
        board.update(input, 1);
    }
}
