#pragma once

#include "eval.h"

class Agent
{
public:
    Agent ();

    Input MakeMove (Board* currentBoard);

private:
    Weights m_weights;
    Move m_workingMove;
    uint8_t m_currentPieceNum;
};
