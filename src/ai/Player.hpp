#pragma once

#include "../game/Board.hpp"

/**
 * PURE ABSTRACT CLASS.
 * An object that can make a move given the current board state.
 * Does not modify the board directly.
 */
class Player {
public:
    /**
     * Generates input given the current board state.
     * @param current_board The current board state.
     * This method does not modify the Board object.
     * @return
     */
    virtual Input gen_input (Board* current_board) = 0;

    virtual ~Player() = default;
};
