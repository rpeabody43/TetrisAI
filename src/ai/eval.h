#pragma once

#include "../game/Board.h"

struct Move
{
    int position;
    int rotation;
    bool hold;
};

struct Weights
{
    double holesCount;
    double aggregateHeight;
    double completeLines;
    double heightStdDev;
};

Move BestMove (Board* currentBoard, Weights& weights);