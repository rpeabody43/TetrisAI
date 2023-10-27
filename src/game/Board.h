#pragma once

#include <random>

#include "tetrominoes.hpp"

struct Input {
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
    static constexpr int WIDTH = 10;
    static constexpr int HEIGHT = 24;
    static constexpr int VISIBLE_HEIGHT = 20;
    static constexpr int VANISH_ZONE_HEIGHT = HEIGHT - VISIBLE_HEIGHT;
    static constexpr int TOTAL_SIZE = WIDTH * HEIGHT;

    /**
     * @param idx A board index.
     * @return What row the index is in.
     */
    static int Row(int idx);


    /**
     * @param idx A board index.
     * @return What column the index is in.
     */
    static int Col(int idx);

    /**
     * Convert typical x, y coordinates to a one-dimensional index.
     * @param x The horizontal coordinate.
     * @param y The vertical coordinate.
     * @return A board index.
     */
    static int IdxConvert(int x, int y);

    /**
	 * Initializes a new game of Tetris.
	 * @param fallRate	The rate at which the pieces naturally fall (lower ->
	 * faster).
	 * @param randomGenerator The engine to generate random numbers with
	 */
	Board(int fallRate, std::default_random_engine randomGenerator);
    
    /**
     * Update the board.
     * @param input The player's inputs.
     * @param ticks The number of milliseconds since initialization
     */
	void Update(Input& inputs, unsigned int ticks);


    /**
     * Gets the lowest possible position the current piece can fall to.
     * @return The anchor of the lowest position
     */
    int GetGhost();

    /**
     * Get the square (cell) associated with a certain x, y coordinate.
     * @param x The horizontal coordinate.
     * @param y The vertical coordinate.
     * @return The value of the square.
     */
	int GetSquare(int x, int y);

    /**
     * Gets the nth piece next up.
     * @param n Which piece to get
     */
    int NthPiece(int delta);

    //region const getters

	/**
	 * Get the nth square position in relation to the anchor of the current piece.
	 * @param rot	The rotation of the piece
	 * @param n		Which square to get
	 */
	[[nodiscard]] int GetPieceMap(int rot, int idx) const;
	
    /**
     * @return The rate the pieces fall.
     */
	[[nodiscard]] unsigned int TicksPerStep() const;

	/**
     * @return The anchor point of the falling piece.
     */
    [[maybe_unused]] [[nodiscard]] int GetFallingPieceAnchor() const;

    /**
     * @return The held piece.
     */
    [[nodiscard]]int GetHeldPiece() const;

    /**
     * @return The falling piece type
     */
    [[nodiscard]] int GetFallingPiece() const;

	/**
     * @return The rotation of the falling piece
     */
    [[nodiscard]] int GetFallingPieceRot() const;

    /**
     * @return The highest point pieces have reached on the board.
     */
    [[nodiscard]] int GetHighestPoint() const;

    /**
     * @return True if the game is over, false otherwise.
     */
    [[nodiscard]] bool GameOver() const;

    //endregion

private:

    /**
     * Executes the hold function.
     * https://tetris.fandom.com/wiki/Hold_piece
     */
	void HoldPiece();

    /**
     *	Move the bag index to the piece after the falling piece.
     *	Called right after creating a new falling piece.
     */
	void NextPiece();

    /**
	 * Creates a new falling piece from the next one up.
	 */
	void NewPiece();

	/**
	 * Creates a new falling piece.
	 * @param piece Which piece to create
	 */
	void NewPiece(int piece);

	
    /**
     * Makes the falling piece fall,
     * freezing and moving onto the next piece if blocked.
     */
	void Fall();

    /**
     * Immedately drop the falling piece.
     */
	void HardDrop();

    /**
     * Checks if a certain move is valid.
     * @param rotDelta How much to rotate the piece
     * @param moveDelta How much to move the piece
     * @return true If the proposed move is legal
     */
	bool ValidMove(int rotDelta, int moveDelta);

	
    /**
     * Move/rotate the current piece a certain amount.
     * DOESN'T DO ANY CHECKS,
     * JUST MOVES PIECE AND LEAVES ZEROES IN ITS PLACE
     * @param rotDelta How much to rotate the piece.
     * @param moveDelta How much to move the piece.
     * @param freeze Whether to freeze the piece and move onto the next one.
     */
	void MovePiece(int rotDelta, int moveDelta, bool freeze);

    /**
     * Move the falling piece and process wall kicks.
     * @param rotDelta How much to rotate the piece.
     * @param moveDelta How much to move the piece.
     */
	void UpdateFallingPiece(int rotDelta, int moveDelta);

    /**
     * Get the correct wall kick for the kind of rotation.
     * @param startRot The initial rotation of the piece.
     * @param endRot The final rotation of the piece.
     * @return The number with which to index into the piece's wall kick
     * table.
     */
	static int GetWallKickIdx(int startRot, int endRot);

	/**
     * Clears any complete lines.
     * Only checks lines that the falling piece takes up.
     */
	void ClearLines();


	bool m_gameover;

	unsigned int m_ticks;
	unsigned int m_lastTicks;
	unsigned int m_fallRate;

	int m_board[TOTAL_SIZE]{};

	int m_fallingPiece;
	int m_fallingPieceRot;
	int m_fallingPieceAnchor;

	int m_bags[2][7];
	// Index of the next piece up
	int m_bagIdx;

	int m_heldPiece;
	bool m_alreadyHeld;

    // Highest point reached on the board.
    // Lower number -> higher because y coordinate is from the top
	int m_currentHighest;

    std::default_random_engine m_randomGenerator;
};