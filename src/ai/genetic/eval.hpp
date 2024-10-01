#pragma once

#include "../../game/Board.hpp"

/* A "move" made up of the final position, rotation, and if a hold was involved */
struct Move {
    int position;
    int rotation;
    bool hold;
};

/* Various weights corresponding to board analysis heuristics */
struct Weights {
    double holes_count;
    double aggregate_height;
    double complete_lines;
    double height_std_dev;
    double highest_point;
    double blocks_over_holes;
};

/**
 * Given the current board state, gets the best possible move.
 * @param current_board The current board state.
 * This method does not modify the Board object.
 * @param weights The set of weights to use for each eval parameter.
 * @return A "Move" with the anchor position, rotation, and whether it's with the held piece.
 */
Move best_move (Board* current_board, Weights& weights);
