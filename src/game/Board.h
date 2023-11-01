#pragma once

#include <random>
#include <cstdint>

#include "tetrominoes.hpp"

struct Input
{
    bool moveLeft;
    bool moveRight;
    bool rotClockwise;
    bool rotCountClockwise;
    bool softDrop;
    bool hardDrop;
    bool holdPiece;
};

class Board
{
public:
    static constexpr uint8_t WIDTH = 10;
    static constexpr uint8_t HEIGHT = 25;
    static constexpr uint8_t VISIBLE_HEIGHT = 20;
    static constexpr uint8_t BUFFER_HEIGHT = 1;
    static constexpr uint8_t VANISH_ZONE_HEIGHT = HEIGHT - VISIBLE_HEIGHT - BUFFER_HEIGHT;
    static constexpr uint16_t TOTAL_SIZE = WIDTH * HEIGHT;

    /**
     * Convert typical x, y coordinates to a one-dimensional index.
     * @param x The horizontal coordinate.
     * @param y The vertical coordinate.
     * @return A board index.
     */
    static uint16_t ConvertIdx (uint8_t x, uint8_t y);

    /**
     * @param idx A board index.
     * @return What row the index is in.
     */
    static uint8_t Row (uint16_t idx);


    /**
     * @param idx A board index.
     * @return What column the index is in.
     */
    static uint8_t Col (uint16_t idx);

    /**
	 * Initializes a new game of Tetris.
	 * @param fallRate	The rate at which the pieces naturally fall (lower ->
	 * faster).
	 * @param randomGenerator The engine to generate random numbers with
	 */
    Board (uint16_t fallRate, std::default_random_engine& randomGenerator);

    /**
     * Update the board.
     * @param input The player's inputs.
     * @param ticks The number of milliseconds since initialization
     */
    void Update (Input& inputs, uint32_t ticks);

    /**
     * Gets the lowest possible position the current piece can fall to.
     * @return The anchor of the lowest position
     */
    [[nodiscard]] uint16_t GetGhost ();

    /**
     * Gets the lowest possible position a certain piece can fall to.
     * @param piece What kind of piece.
     * @param anchor The initial anchor of the piece.
     * @param currentRot The rotation of the piece.
     * @return The anchor of the lowest position.
     */
    [[nodiscard]] uint16_t GetGhost (uint8_t piece, uint16_t anchor, uint8_t currentRot);

    /**
     * Get the square (cell) associated with a certain x, y coordinate.
     * @param x The horizontal coordinate.
     * @param y The vertical coordinate.
     * @return The value of the square.
     */
    [[nodiscard]] int8_t GetSquare (uint8_t x, uint8_t y) const;

    /**
     * Get the square (cell) associated with a certain index
     * @param idx The index of the square
     * @return The value of the square.
     */
    [[nodiscard]] int8_t GetSquare (uint16_t idx) const;

    /**
     * Gets the nth piece next up.
     * @param n Which piece to get
     */
    [[nodiscard]] uint8_t NthPiece (uint8_t delta) const;

    /**
     * Gets the current number piece in the bags
     * @return A number between 0 and 13, as there are two bags of 7 that are shuffled/swapped
     */
    [[nodiscard]] uint8_t GetPieceNum () const;

    //region const getters

    /**
     * Get the nth square position in relation to the anchor of the current piece.
     * @param rot	The rotation of the piece
     * @param n		Which square to get
     */
    [[nodiscard]] uint8_t GetPieceMap (uint8_t rot, uint8_t n) const;

    /**
     * @return The anchor point of the falling piece.
     */
    [[nodiscard]] uint16_t GetFallingPieceAnchor () const;

    /**
     * @return The held piece.
     */
    [[nodiscard]] uint8_t GetHeldPiece () const;

    /**
     * @return The falling piece type
     */
    [[nodiscard]] uint8_t GetFallingPiece () const;

    /**
     * @return The rotation of the falling piece
     */
    [[nodiscard]] uint8_t GetFallingPieceRot () const;

    /**
     * @return The highest point pieces have reached on the board.
     */
    [[nodiscard]] uint8_t GetHighestRow () const;

    /**
     * @return True if the game is over, false otherwise.
     */
    [[nodiscard]] bool GameOver () const;

    /**
     * @return The current score of the game
     */
    [[nodiscard]] size_t GetScore () const;

    /**
     * @return How many lines have been cleared so far.
     */
    [[nodiscard]] size_t GetLinesCleared() const;

    //endregion

private:

    /**
     * Executes the hold function.
     * https://tetris.fandom.com/wiki/Hold_piece
     */
    void HoldPiece ();

    /**
     *	Move the bag index to the piece after the falling piece.
     *	Called right after creating a new falling piece.
     */
    void NextPiece ();

    /**
	 * Creates a new falling piece from the next one up.
	 */
    void NewPiece ();

    /**
     * Creates a new falling piece.
     * @param piece Which piece to create
     */
    void NewPiece (uint8_t piece);


    /**
     * Makes the falling piece fall,
     * freezing and moving onto the next piece if blocked.
     */
    void Fall ();

    /**
     * Immedately drop the falling piece.
     */
    void HardDrop ();

    /**
     * Checks if a certain move from the current piece rotation and position is valid.
     * @param rotDelta How much to rotate the piece
     * @param moveDelta How much to move the piece
     * @return true If the proposed move is legal
     */
    bool ValidMove (int8_t rotDelta, int16_t moveDelta);

    /**
     * Checks if a certain move is valid with the current board.
     * PRECONDITION: The piece is in a valid area to begin with
     * @param piece What type of piece.
     * @param anchor Where the piece anchor is initially.
     * @param currentRot The piece's rotation initially
     * @param rotDelta How much to rotate the piece
     * @param moveDelta How much to move the piece
     * @return true If the proposed move is legal
     */
    bool ValidMove (uint8_t piece, uint16_t anchor, uint8_t currentRot, int8_t rotDelta, int16_t moveDelta);


    /**
     * Move/rotate the current piece a certain amount.
     * DOESN'T DO ANY CHECKS,
     * JUST MOVES PIECE AND LEAVES ZEROES IN ITS PLACE
     * @param rotDelta How much to rotate the piece.
     * @param moveDelta How much to move the piece.
     * @param freeze Whether to freeze the piece and move onto the next one.
     */
    void MovePiece (int8_t rotDelta, int16_t moveDelta, bool freeze);

    /**
     * Move the falling piece and process wall kicks.
     * @param rotDelta How much to rotate the piece.
     * @param moveDelta How much to move the piece.
     */
    void UpdateFallingPiece (int8_t rotDelta, int16_t moveDelta);

    /**
     * Get the correct wall kick for the kind of rotation.
     * @param startRot The initial rotation of the piece.
     * @param endRot The final rotation of the piece.
     * @return The number with which to index into the piece's wall kick
     * table.
     */
    static uint8_t GetWallKickIdx (uint8_t startRot, uint8_t endRot);

    /**
     * Clears any complete lines.
     * Only checks lines that the falling piece takes up.
     */
    void ClearLines ();


    bool m_gameover;

    uint32_t m_ticks;
    uint32_t m_lastTicks;
    uint16_t m_fallRate;

    int8_t m_board[TOTAL_SIZE]{};

    uint8_t m_fallingPiece;
    uint8_t m_fallingPieceRot;
    uint16_t m_fallingPieceAnchor;

    uint8_t m_bags[2][7];
    // Index of the next piece up
    uint8_t m_bagIdx;

    uint8_t m_heldPiece;
    bool m_alreadyHeld;

    // Highest point reached on the board.
    // Lower number -> higher because y coordinate is from the top
    uint8_t m_currentHighest;

    size_t m_score;
    size_t m_linesCleared;

    std::default_random_engine& m_randomGenerator;
};