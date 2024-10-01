#pragma once

#include <random>
#include <cstdint>

#include "tetrominoes.hpp"

struct Input {
    bool move_left;
    bool move_right;
    bool rot_clockwise;
    bool rot_count_clockwise;
    bool soft_drop;
    bool hard_drop;
    bool hold_piece;
};

/* Contains the state of the Tetris game */
class Board {
public:
    static constexpr uint8_t WIDTH = 10;
    static constexpr uint8_t HEIGHT = 25;
    static constexpr uint8_t VISIBLE_HEIGHT = 20;
    static constexpr uint8_t BUFFER_HEIGHT = 1;
    static constexpr uint8_t BUFFER_SQUARES = BUFFER_HEIGHT*WIDTH;
    static constexpr uint8_t VANISH_ZONE_HEIGHT = HEIGHT - VISIBLE_HEIGHT - BUFFER_HEIGHT;
    static constexpr uint16_t TOTAL_SIZE = WIDTH * HEIGHT;

    /**
     * Convert typical x, y coordinates to a one-dimensional index.
     * @param x The horizontal coordinate.
     * @param y The vertical coordinate.
     * @return A board index.
     */
    static uint16_t convert_idx (uint8_t x, uint8_t y);

    /**
     * @param idx A board index.
     * @return What row the index is in.
     */
    static uint8_t row (uint16_t idx);


    /**
     * @param idx A board index.
     * @return What column the index is in.
     */
    static uint8_t col (uint16_t idx);

    /**
	 * Initializes a new game of Tetris.
	 * @param fall_rate	The rate at which the pieces naturally fall (lower ->
	 * faster).
	 * @param random_generator The engine to generate random numbers with
	 */
    Board (uint16_t fall_rate, std::default_random_engine& random_generator);

    /**
     * Update the board.
     * @param input The player's inputs.
     * @param ticks The number of milliseconds since initialization
     */
    void update (Input& inputs, uint32_t ticks);

    /**
     * Gets the lowest possible position the current piece can fall to.
     * @return The anchor of the lowest position
     */
    [[nodiscard]] uint16_t get_ghost ();

    /**
     * Gets the lowest possible position a certain piece can fall to.
     * @param piece What kind of piece.
     * @param anchor The initial anchor of the piece.
     * @param current_rot The rotation of the piece.
     * @return The anchor of the lowest position.
     */
    [[nodiscard]] uint16_t get_ghost (uint8_t piece, uint16_t anchor, uint8_t current_rot);

    /**
     * Get the square (cell) associated with a certain x, y coordinate.
     * @param x The horizontal coordinate.
     * @param y The vertical coordinate.
     * @return The value of the square.
     */
    [[nodiscard]] int8_t get_square (uint8_t x, uint8_t y) const;

    /**
     * Get the square (cell) associated with a certain index
     * @param idx The index of the square
     * @return The value of the square.
     */
    [[nodiscard]] int8_t get_square (uint16_t idx) const;

    /**
     * Gets the nth piece next up.
     * @param n Which piece to get
     */
    [[nodiscard]] uint8_t nth_piece (uint8_t delta) const;

    /**
     * Gets the current number piece in the bags
     * @return A number between 0 and 13, as there are two bags of 7 that are shuffled/swapped
     */
    [[nodiscard]] uint8_t get_piece_num () const;

    //region const getters

    /**
     * Get the nth square position in relation to the anchor of the current piece.
     * @param rot	The rotation of the piece
     * @param n		Which square to get
     */
    [[nodiscard]] uint8_t get_piece_map (uint8_t rot, uint8_t n) const;

    /**
     * @return The anchor point of the falling piece.
     */
    [[nodiscard]] uint16_t get_falling_piece_anchor () const;

    /**
     * @return The held piece.
     */
    [[nodiscard]] uint8_t get_held_piece () const;

    /**
     * @return The falling piece type
     */
    [[nodiscard]] uint8_t get_falling_piece () const;

    /**
     * @return The rotation of the falling piece
     */
    [[nodiscard]] uint8_t get_falling_piece_rot () const;

    /**
     * @return The highest point pieces have reached on the board.
     */
    [[nodiscard]] uint8_t get_highest_row () const;

    /**
     * @return True if the game is over, false otherwise.
     */
    [[nodiscard]] bool game_over () const;

    /**
     * @return The current score of the game
     */
    [[nodiscard]] size_t get_score () const;

    /**
     * @return How many lines have been cleared so far.
     */
    [[nodiscard]] size_t get_lines_cleared() const;

    //endregion

private:

    /**
     * Executes the hold function.
     * https://tetris.fandom.com/wiki/Hold_piece
     */
    void hold_piece ();

    /**
     *	Move the bag index to the piece after the falling piece.
     *	Called right after creating a new falling piece.
     */
    void next_piece ();

    /**
	 * Creates a new falling piece from the next one up.
	 */
    void new_piece ();

    /**
     * Creates a new falling piece.
     * @param piece Which piece to create
     */
    void new_piece (uint8_t piece);


    /**
     * Makes the falling piece fall,
     * freezing and moving onto the next piece if blocked.
     */
    void fall ();

    /**
     * Immedately drop the falling piece.
     */
    void hard_drop ();

    /**
     * Checks if a certain move from the current piece rotation and position is valid.
     * @param rot_delta How much to rotate the piece
     * @param move_delta How much to move the piece
     * @return true If the proposed move is legal
     */
    bool valid_move (int8_t rot_delta, int16_t move_delta);

    /**
     * Checks if a certain move is valid with the current board.
     * PRECONDITION: The piece is in a valid area to begin with
     * @param piece What type of piece.
     * @param anchor Where the piece anchor is initially.
     * @param current_rot The piece's rotation initially
     * @param rot_delta How much to rotate the piece
     * @param move_delta How much to move the piece
     * @return true If the proposed move is legal
     */
    bool valid_move (
        uint8_t piece, uint16_t anchor, uint8_t current_rot, 
        int8_t rot_delta, int16_t move_delta
    );

    /**
     * Move/rotate the current piece a certain amount.
     * DOESN'T DO ANY CHECKS,
     * JUST MOVES PIECE AND LEAVES ZEROES IN ITS PLACE
     * @param rot_delta How much to rotate the piece.
     * @param move_delta How much to move the piece.
     * @param freeze Whether to freeze the piece and move onto the next one.
     */
    void move_piece (int8_t rot_delta, int16_t move_delta, bool freeze);

    /**
     * Move the falling piece and process wall kicks.
     * @param rot_delta How much to rotate the piece.
     * @param move_delta How much to move the piece.
     */
    void update_falling_piece (int8_t rot_delta, int16_t move_delta);

    /**
     * Get the correct wall kick for the kind of rotation.
     * @param start_rot The initial rotation of the piece.
     * @param end_rot The final rotation of the piece.
     * @return The number with which to index into the piece's wall kick
     * table.
     */
    static uint8_t get_wall_kick_idx (uint8_t start_rot, uint8_t end_rot);

    /**
     * Clears any complete lines.
     * Only checks lines that the falling piece takes up.
     */
    void clear_lines ();


    bool m_gameover;

    uint32_t m_ticks;
    uint32_t m_last_ticks;
    uint16_t m_fall_rate;

    int8_t m_board[TOTAL_SIZE]{};

    uint8_t m_falling_piece;
    uint8_t m_falling_piece_rot;
    uint16_t m_falling_piece_anchor;

    uint8_t m_bags[2][7];
    // Index of the next piece up
    uint8_t m_bag_idx;

    uint8_t m_held_piece;
    bool m_already_held;

    // Highest point reached on the board.
    // Lower number -> higher because y coordinate is from the top
    uint8_t m_current_highest;

    size_t m_score;
    size_t m_lines_cleared;

    std::default_random_engine& m_randomgen;
};
