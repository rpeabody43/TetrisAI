#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include "../game/Board.h"

/* Draws the game of Tetris to the screen. */
class GameWindow
{
public:
    static constexpr uint8_t OFFSCREEN_ROWS = Board::VANISH_ZONE_HEIGHT + Board::BUFFER_HEIGHT;
    static constexpr uint8_t OFFSCREEN_SQUARES = Board::WIDTH*OFFSCREEN_ROWS;

    /**
     * Constructor: creates a new Window.
     * Run Init() afterwards.
     * @param screenW The width of the window.
     * @param screenH The height of the window.
     */
    GameWindow (uint16_t screenW, uint16_t screenH);

    /**
     * Initializes the window and the SDL things it depends on.
     * @return True if everything goes well, false otherwise.
     */
    bool Init ();

    /**
     * Draw a frame to the screen.
     * @param currentBoard The current game board.
     */
    void Draw (Board* currentBoard);

    /**
     * Destructor: safely closes and exits the window..
     */
    ~GameWindow ();

private:
    /*
     * Prevent the app from scaling on UNIX-based OSs.
     * I HAVE NOT TESTED THIS.
     */
    bool UnixScaling ();

    /**
     * Draw a piece not on the board in a specific spot on the screen.
     * Centers I and O.
     * @param x The x coordinate to begin drawing the piece.
     * @param y The y coordinate to begin drawing the piece.
     * @param piece What kind of piece to draw.
     * @param rot What rotation the piece should have.
     * @param sqSize The size of each square in the piece.
     */
    void DrawPiece (uint16_t x, uint16_t y, uint8_t piece, uint8_t rot, uint8_t sqSize);

    /**
     * Draws the ghost piece (where the piece would land on a hard drop) at the bottom of the screen.
     * @param currentBoard The current game board.
     * @param xOffset The x position of the board itself on the screen.
     * @param yOffset The y position of the board itself on the screen.
     * @param piece What kind of piece to draw.
     * @param rot What rotation the piece should have.
     * @param sqSize The size of each square in the piece
     */
    void DrawGhostPiece (Board* currentBoard, uint16_t xOffset, uint16_t yOffset, uint8_t piece, uint8_t rot, uint8_t sqSize);

    /**
     * Draw some text to the screen.
     * @param x The x position to center the text.
     * @param y The y position to center the text.
     * @param txt The text to draw.
     * @param font What SDL font to use.
     * @param color What color the text should be.
     */
    void DrawTxt (uint16_t x, uint16_t y, const char* txt, TTF_Font* font, SDL_Color color);

    void DrawScore (int x, int y);

    uint16_t m_screenW;
    uint16_t m_screenH;

    SDL_Renderer* m_pRenderer;
    SDL_Window* m_pWindow;
    TTF_Font* m_pFont28;
    TTF_Font* m_pFont40;
};
