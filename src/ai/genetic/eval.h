#pragma once

#include "../../game/Board.h"

/* A "move" made up of the final position, rotation, and if a hold was involved */
struct Move
{
    int position;
    int rotation;
    bool hold;
};

/* Various weights corresponding to board analysis heuristics */
struct Weights
{
    double holesCount;
    double aggregateHeight;
    double completeLines;
    double heightStdDev;
    double highestPoint;
    double blocksOverHoles;
};

/**
 * Given the current board state, gets the best possible move.
 * @param currentBoard The current board state.
 * This method does not modify the Board object.
 * @param weights The set of weights to use for each eval parameter.
 * @return A "Move" with the anchor position, rotation, and whether it's with the held piece.
 */
Move BestMove (Board* currentBoard, Weights& weights);
