#pragma once

#include "eval.hpp"
#include "../Player.hpp"

/* The player in the genetic algorithm */
class Agent : public Player {
public:
    /**
     * Creates a new Agent with specific weights.
     * @param hard_drop Whether the agent should always hard drop or always soft drop.
     * @param weights This agent's weights.
     */
    Agent (bool hard_drop, Weights weights);

    Input gen_input (Board* current_board) override;

    /**
     * Generates the fitness of the agent by playing Tetris
     */
    void calc_fitness ();

private:
    Weights m_weights;
    size_t m_fitness;

    Move m_working_move;
    uint8_t m_current_piece_num;
    bool m_hard_drop;
};
