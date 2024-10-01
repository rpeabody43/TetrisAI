// For randomization
#include <algorithm>
#include <random>

#include "Board.hpp"
#include "tetrominoes.hpp"

uint16_t Board::convert_idx (uint8_t x, uint8_t y) {
    return y * WIDTH + x;
}

uint8_t Board::row (uint16_t idx) {
    return idx / WIDTH;
}

uint8_t Board::col (uint16_t idx) {
    return idx % WIDTH;
}

Board::Board (uint16_t fall_rate, std::default_random_engine& random_generator) // NOLINT(*-msc51-cpp)
    : m_gameover(false)
    , m_ticks(0)
    , m_last_ticks(0)
    , m_fall_rate(fall_rate)
    , m_falling_piece(0)
    , m_falling_piece_rot(0)
    , m_falling_piece_anchor(3)
    , m_board{}
    , m_bags{}
    , m_bag_idx(7)
    , m_held_piece(0)
    , m_already_held(false)
    , m_current_highest(HEIGHT)
    , m_score(0)
    , m_lines_cleared(0)
    , m_randomgen(random_generator) {
    // Initialize each bag in sequential order, then shuffle
    for (auto& bag: m_bags) {
        for (int j = 0; j < 7; j++)
            bag[j] = j + 1;
        std::shuffle(std::begin(bag), std::end(bag), m_randomgen);
    }
}

uint8_t Board::get_piece_map (uint8_t rot, uint8_t n) const {
    uint8_t ret = tetromino_data::get_piece_map(m_falling_piece, rot, n);
    return ret;
}

uint8_t Board::nth_piece (uint8_t n) const {
    uint8_t idx = m_bag_idx + n;
    if (idx >= sizeof(m_bags))
        idx -= sizeof(m_bags);
    return m_bags[idx / 7][idx % 7];
}

uint8_t Board::get_piece_num () const {
    return m_bag_idx;
}

void Board::next_piece () {
    // If reached the end of the current piece bag, shuffle it and move onto the next;
    if ((m_bag_idx + 1) % 7 == 0) {
        std::shuffle(
            std::begin(m_bags[m_bag_idx / 7]), 
            std::end(m_bags[m_bag_idx / 7]), m_randomgen
        );
    }
    m_bag_idx++;
    if (m_bag_idx >= sizeof(m_bags))
        m_bag_idx = 0;
}

void Board::new_piece () {
    int piece = nth_piece(0);
    new_piece(piece);
}


void Board::new_piece (uint8_t piece) {
    m_falling_piece = piece;
    m_falling_piece_rot = 0;
    // If the highest point is just below the vanish zone
    // Spawn the piece in the vanish zone
    if (m_current_highest <= VANISH_ZONE_HEIGHT + 2) {
        m_falling_piece_anchor = convert_idx(3, BUFFER_HEIGHT);
    } else { // Otherwise spawn in visible space
        m_falling_piece_anchor = convert_idx(3, VANISH_ZONE_HEIGHT + BUFFER_HEIGHT);
    }

    // Move up in the bag
    next_piece();

    uint16_t start = m_falling_piece_anchor;
    // _pieces spawn on top of other pieces
    bool blockOut = false;

    // Set all falling piece squares to the negative value of the piece
    for (int i = 3; i >= 0; i--) {
        uint16_t newIdx = start + get_piece_map(m_falling_piece_rot, i);

        if (m_board[newIdx] == 0) {
            m_board[newIdx] = -m_falling_piece;
        } else {
            blockOut = true;
        }
    }

    m_gameover = blockOut;
}

void Board::hold_piece () {
    if (m_already_held) return;
    for (int i = 3; i >= 0; i--) {
        int idx = m_falling_piece_anchor + get_piece_map(m_falling_piece_rot, i);
        m_board[idx] = 0;
    }

    int prev_held_piece = m_held_piece;
    m_held_piece = m_falling_piece;
    if (prev_held_piece == 0)
        new_piece();
    else
        new_piece(prev_held_piece);
    m_already_held = true;
}

void Board::clear_lines () {
    const uint16_t start = m_falling_piece_anchor;
    const uint8_t start_row = row(start);
    uint8_t lines_cleared = 0;

    // Copy lines down to cover cleared lines
    for (int y = start_row; y < std::min(start_row + 5, (int) HEIGHT); y++) {
        bool line_complete = true;
        for (int x = 0; x < 10 && line_complete; x++) {
            if (get_square(x, y) == 0)
                line_complete = false;
        }

        if (line_complete) {
            lines_cleared++;

            for (int temp_y = y - 1; temp_y >= start_row - 1; temp_y--) {
                for (int temp_x = 0; temp_x < 10; temp_x++) {
                    int currentIdx = convert_idx(temp_x, temp_y);
                    int copyIdx = convert_idx(temp_x, temp_y + 1);
                    m_board[copyIdx] = m_board[currentIdx];
                }
            }
        }
    }

    if (lines_cleared == 0) return;
    // Copy down the rest of the lines to
    for (int temp_y = start_row - 1; temp_y >= 0; temp_y--) {
        for (int temp_x = 0; temp_x < 10; temp_x++) {
            uint16_t current_idx = convert_idx(temp_x, temp_y);
            uint16_t copy_idx = convert_idx(temp_x, temp_y + lines_cleared);
            m_board[copy_idx] = m_board[current_idx];
        }
    }

    // Fill the top with zeroes
    for (int buffer = 0; buffer < convert_idx(9, lines_cleared); buffer++) {
        m_board[buffer] = 0;
    }

    // Since y starts from the top, currentHighest needs to be increased
    m_current_highest += lines_cleared;

    m_lines_cleared += lines_cleared;
    uint16_t score_add;
    switch (lines_cleared) {
        default:
            score_add = 0;
            break;
        case 1:
            score_add = 100;
            break;
        case 2:
            score_add = 300;
            break;
        case 3:
            score_add = 500;
            break;
        case 4:
            score_add = 800;
    }
    m_score += score_add*m_lines_cleared/10;
}

void Board::fall () {
    m_last_ticks = m_ticks;

    bool freeze = !valid_move(0, 10);
    if (freeze) {
        move_piece(0, 0, true);
        clear_lines();
        new_piece();
        return;
    }

    update_falling_piece(0, 10);
}

uint16_t Board::get_ghost () {
    return get_ghost(
        m_falling_piece, m_falling_piece_anchor, m_falling_piece_rot
    );
}

uint16_t Board::get_ghost (
    uint8_t piece, uint16_t anchor, uint8_t current_rot
) {
    uint16_t delta = 0;
    while (valid_move(piece, anchor, current_rot, 0, delta + Board::WIDTH))
        delta += Board::WIDTH;
    return anchor + delta;
}

void Board::hard_drop () {
    uint16_t move_delta = get_ghost() - m_falling_piece_anchor;
    move_piece(0, move_delta, true);
    clear_lines();
    new_piece();
}

bool Board::valid_move (int8_t rot_delta, int16_t move_delta) {
    return valid_move(
        m_falling_piece, m_falling_piece_anchor, m_falling_piece_rot,
        rot_delta, move_delta
    );
}

bool Board::valid_move (
    uint8_t piece, uint16_t anchor, uint8_t current_rot, 
    int8_t rot_delta, int16_t move_delta
) {
    for (int i = 0; i < 4; i++) {
        int old_pos = anchor + tetromino_data::get_piece_map(piece, current_rot, i);
        int new_pos = anchor + move_delta + tetromino_data::get_piece_map(piece, current_rot + rot_delta, i);
        // If it's an index error on either side
        if (new_pos >= TOTAL_SIZE || new_pos < 0)
            return false;
        // If it's a nonempty square, that also isn't the falling piece itself
        // The falling piece is stored as negative numbers
        if (m_board[new_pos] != 0 && m_board[new_pos] != -m_falling_piece)
            return false;
        // If it hit the side of the board
        int old_col = col(old_pos);
        int new_col = col(new_pos);
        bool closeToEdge = ((old_col <= 2) && (new_col >= 8)) || ((old_col >= 8) && (new_col <= 2));
        if (closeToEdge)
            return false;
    }
    return true;
}

void Board::move_piece (int8_t rot_delta, int16_t move_delta, bool freeze) {
    for (int i = 3; i >= 0; i--) {
        // get the block with the delta from the map array
        int abs_idx_old = m_falling_piece_anchor + 
            get_piece_map(m_falling_piece_rot, i);
        m_board[abs_idx_old] = 0;
    }
    for (int i = 3; i >= 0; i--) {
        uint16_t abs_idx_new = m_falling_piece_anchor + move_delta + 
            get_piece_map(m_falling_piece_rot + rot_delta, i);
        m_board[abs_idx_new] = freeze ? m_falling_piece : -m_falling_piece;
    }

    m_falling_piece_rot += rot_delta;
    m_falling_piece_anchor += move_delta;

    if (freeze) {
        // The held piece becomes available when the current falling piece is locked
        m_already_held = false;

        // We use > because y is from top down
        if (m_current_highest > row(m_falling_piece_anchor))
            m_current_highest = row(m_falling_piece_anchor);

        // Lock out
        if (
            m_falling_piece_anchor + get_piece_map(m_falling_piece_rot, 3) 
            < convert_idx(0, VANISH_ZONE_HEIGHT)
        ) {
            m_gameover = true;
        }
    }
}

void Board::update_falling_piece (int8_t rot_delta, int16_t move_delta)
{
    // Do movement first because we don't want it to stack with wall kicks
    if (valid_move(0, move_delta))
        move_piece(0, move_delta, false);

    // Make sure m_falling_piece_rot is between 0 and 3
    while (m_falling_piece_rot + rot_delta < 0)
        rot_delta += 4;
    while (m_falling_piece_rot + rot_delta > 3)
        rot_delta -= 4;

    int wall_kick_table = get_wall_kick_idx(
        m_falling_piece_rot, 
        m_falling_piece_rot + rot_delta
    );
    int i = 0;
    // O pieces should not be rotated / wall kicked at all
    if (m_falling_piece == O_PIECE || rot_delta == 0) {
        return;
    } else {
        // Loop through the wall kicks at this rotation until one works, 
        // or they all fail
        if (m_falling_piece == I_PIECE) {
            // The I piece has a different table of wall kicks per SRS
            while (
                i < 5 && !valid_move(
                    rot_delta, tetromino_data::I_WALL_KICKS[wall_kick_table][i]
                )
            ) {
                i++;
            }
        } else {
            while (
                i < 5 && !valid_move(
                    rot_delta, tetromino_data::WALL_KICKS[wall_kick_table][i]
                )
            ) {
                i++;
            }
        }
    }

    if (i != 5) {
        int8_t wall_kick;
        if (m_falling_piece == I_PIECE) 
            wall_kick = tetromino_data::I_WALL_KICKS[wall_kick_table][i];
        else
            wall_kick = tetromino_data::WALL_KICKS[wall_kick_table][i];
        move_piece(rot_delta, wall_kick, false);
    }
}

uint8_t Board::get_wall_kick_idx (uint8_t start_rot, uint8_t end_rot)
{
    if (start_rot == 3 && end_rot == 0)
        return 6;
    else if (start_rot == 0 && end_rot == 3)
        return 7;

    int dir = end_rot - start_rot;
    int ret = start_rot + end_rot;
    if (dir == 1)
        ret -= 1;
    return ret;
}

[[maybe_unused]] uint16_t Board::get_falling_piece_anchor () const
{
    return m_falling_piece_anchor;
}

void Board::update (Input& input, uint32_t ticks)
{
    // m_falling_piece is only assigned 0 at new game
    // every other piece's number is > 0
    if (m_falling_piece == 0) {
        new_piece();
        return;
    }
    if (m_gameover) return;
    m_ticks = ticks;
    if (m_ticks - m_last_ticks >= m_fall_rate) {
        fall();
    }

    int16_t move_delta = 0;
    int8_t rot_delta = 0;

    if (input.move_left)
        move_delta -= 1;
    if (input.move_right)
        move_delta += 1;
    if (input.soft_drop)
        fall();
    if (input.rot_clockwise)
        rot_delta += 1;
    if (input.rot_count_clockwise)
        rot_delta -= 1;
    if (input.hard_drop)
        hard_drop();
    if (input.hold_piece)
        hold_piece();

    update_falling_piece(rot_delta, move_delta);

}

uint8_t Board::get_falling_piece () const
{
    return m_falling_piece;
}

uint8_t Board::get_falling_piece_rot () const
{
    return m_falling_piece_rot;
}

int8_t Board::get_square (uint8_t x, uint8_t y) const
{
    return m_board[convert_idx(x, y)];
}

int8_t Board::get_square (uint16_t idx) const
{
    return m_board[idx];
}

uint8_t Board::get_held_piece () const
{
    return m_held_piece;
}

uint8_t Board::get_highest_row () const
{
    return m_current_highest;
}

bool Board::game_over () const
{
    return m_gameover;
}

size_t Board::get_score () const
{
    return m_score;
}

size_t Board::get_lines_cleared () const
{
    return m_lines_cleared;
}
