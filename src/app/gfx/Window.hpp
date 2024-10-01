#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "../../game/Board.hpp"

/* Draws the game of Tetris to the screen. */
class GameWindow {
public:
    static constexpr uint8_t OFFSCREEN_ROWS = 
        Board::VANISH_ZONE_HEIGHT + Board::BUFFER_HEIGHT;

    static constexpr uint8_t OFFSCREEN_SQUARES = 
        Board::WIDTH * OFFSCREEN_ROWS;

    /**
     * Constructor: creates a new Window.
     * Run init() afterwards.
     */
    GameWindow ();

    /**
     * Initializes the window and the SDL things it depends on.
     * @return True if everything goes well, false otherwise.
     */
    bool init ();

    /**
     * Draw a frame to the screen.
     * @param current_board The current game board.
     */
    void draw (Board* current_board);

    /**
     * Destructor: safely closes and exits the window..
     */
    ~GameWindow ();

private:
    /*
     * Prevent the app from scaling on UNIX-based OSs.
     * I HAVE NOT TESTED THIS.
     */
    bool unix_scaling ();

    /**
     * Draw a piece not on the board in a specific spot on the screen.
     * Centers I and O.
     * @param x The x coordinate to begin drawing the piece.
     * @param y The y coordinate to begin drawing the piece.
     * @param piece What kind of piece to draw.
     * @param rot What rotation the piece should have.
     * @param sq_size The size of each square in the piece.
     */
    void draw_piece (
        uint16_t x, uint16_t y, uint8_t piece, uint8_t rot, uint8_t sq_size
    );

    /**
     * Draws the ghost piece (where the piece would land on a hard drop) at the bottom of the screen.
     * @param current_board The current game board.
     * @param x_offset The x position of the board itself on the screen.
     * @param y_offset The y position of the board itself on the screen.
     * @param piece What kind of piece to draw.
     * @param rot What rotation the piece should have.
     * @param sq_size The size of each square in the piece
     */
    void draw_ghost_piece (
        Board* current_board, uint16_t x_offset, uint16_t y_offset,
        uint8_t piece, uint8_t rot, uint8_t sq_size
    );

    /**
     * Draw a value like to the screen with a label like "SCORE" centered above it.
     * @param x The x coordinate to center the text.
     * @param y The y coordinate to center the label text.
     * @param offset How far the value should be from the label.
     * @param label The label to draw.
     * @param number The value to draw.
     * @param font What SDL font to use.
     * @param color What color the text should be.
     */
    void draw_labeled_number (
        uint16_t x, uint16_t y, uint8_t offset, const char* label,
        size_t number, TTF_Font* font, SDL_Color color
    );

    /**
     * Draw some text to the screen.
     * @param x The x position to center the text.
     * @param y The y position to center the text.
     * @param txt The text to draw.
     * @param font What SDL font to use.
     * @param color What color the text should be.
     */
    void draw_txt (
        uint16_t x, uint16_t y, const char* txt, TTF_Font* font, SDL_Color color
    );

    void draw_score (int x, int y);

    SDL_Renderer* m_renderer;
    SDL_Window* m_window;
    TTF_Font* m_font28;
    TTF_Font* m_font40;
};
