#include "Agent.hpp"

Agent::Agent (bool hard_drop, Weights weights)
    : m_weights(weights)
    , m_working_move({})
    , m_current_piece_num(14)
    , m_fitness()
    , m_hard_drop(hard_drop)
{}

Input Agent::gen_input (Board* current_board)
{
    Input input = {};
    if (m_current_piece_num != current_board->get_piece_num())
    {
        m_working_move = best_move(current_board, m_weights);
        m_current_piece_num = current_board->get_piece_num();
    }

    if (m_working_move.hold)
    {
        input.hold_piece = true;
        m_working_move.hold = false;
        return input;
    }

    uint8_t current_piece = current_board->get_falling_piece();
    uint8_t current_rot = current_board->get_falling_piece_rot();

    int8_t rot_delta = m_working_move.rotation - current_rot;
    if (rot_delta < 0)
    {
        input.rot_count_clockwise = true;
    }
    else if (rot_delta > 0)
    {
        input.rot_clockwise = true;
    }

    uint16_t current_anchor = current_board->get_falling_piece_anchor();
    // int8_t rowDelta = Board::Row(m_working_move.position) - Board::Row(current_anchor);
    int8_t offset = tetromino_data::get_piece_bounds(
        current_piece, current_rot
    ).left_bound;

    uint8_t target_col = Board::col(m_working_move.position - offset);
    uint8_t current_col = Board::col(current_anchor - offset);
    int8_t col_delta = target_col - current_col;
    if (col_delta < 0)
    {
        input.move_left = true;
    }
    else if (col_delta > 0)
    {
        input.move_right = true;
    }
    else
    {
        if (m_hard_drop)
        {
            if (rot_delta == 0)
            {
                input.hard_drop = true;
            }
        }
        else
        {
            input.soft_drop = true;
        }
    }


    return input;
}

void Agent::set_fitness (size_t fitness) {
    m_fitness = fitness;
}

size_t Agent::get_fitness () const {
    return m_fitness;
}

Weights Agent::get_weights () const {
    return m_weights;
}
