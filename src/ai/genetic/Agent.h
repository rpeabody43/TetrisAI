#pragma once

#include "eval.h"
#include "../Player.h"

/* The player in the genetic algorithm */
class Agent : public Player
{
public:
    /**
     * Creates a new Agent with specific weights.
     * @param hardDrop Whether the agent should always hard drop or always soft drop.
     * @param weights This agent's weights.
     */
    Agent (bool hardDrop, Weights weights);

    Input GenInput (Board* currentBoard) override;

    /**
     * Generates the fitness of the agent by playing Tetris
     */
    void CalcFitness ();

private:
    Weights m_weights;
    size_t m_fitness;

    Move m_workingMove;
    uint8_t m_currentPieceNum;
    bool m_hardDrop;
};
