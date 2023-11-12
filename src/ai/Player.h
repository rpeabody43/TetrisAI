#pragma once

#include "../game/Board.h"

/**
 * PURE ABSTRACT CLASS.
 * An object that can make a move given the current board state.
 * Does not modify the board directly.
 */
class Player
{
public:
    /**
     * Generates input given the current board state.
     * @param currentBoard The current board state.
     * This method does not modify the Board object.
     * @return
     */
    virtual Input GenInput (Board* currentBoard) = 0;

    virtual ~Player() = default;
};