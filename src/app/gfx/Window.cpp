#include <iostream>
#include <string>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#if defined(_WIN32) || defined(WIN32)

#include <windows.h> // To disable Windows scaling

#define OS_WINDOWS

#endif

#include "Window.h"


constexpr uint16_t WINDOW_W = 1280;
constexpr uint16_t WINDOW_H = 960;


GameWindow::GameWindow ()
    : m_pRenderer(nullptr)
      , m_pWindow(nullptr)
      , m_pFont28(nullptr)
      , m_pFont40(nullptr)
{}

bool GameWindow::UnixScaling ()
{
    int rw = 0, rh = 0;
    SDL_GetCurrentRenderOutputSize(m_pRenderer, &rw, &rh);

    std::cout << "rw: " << rw << ", rh: " << rh << std::endl;

    if (rw != WINDOW_W)
    {
        float wScale = (float) rw / (float) WINDOW_W;
        float hScale = (float) rh / (float) WINDOW_H;

        if (wScale != hScale)
        {
            std::cout << "SCALING ERROR: Width scale != Height scale"
                      << std::endl;
            return false;
        }

        SDL_SetRenderScale(m_pRenderer, wScale, hScale);
    }
    return true;
}

bool GameWindow::Init ()
{

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "SDL FAILED TO INITIALIZE: " << SDL_GetError()
                  << std::endl;
        return false;
    }

    TTF_Init();

    // Fix for non-native scaling/DPI (150% in Windows, etc.)
#ifdef OS_WINDOWS
    SetProcessDPIAware();
#endif

    m_pWindow = SDL_CreateWindow
        (
            "TetrisAI",
            WINDOW_W,
            WINDOW_H,
            SDL_WINDOW_HIGH_PIXEL_DENSITY
        );

    m_pRenderer = SDL_CreateRenderer(m_pWindow, nullptr);
    SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);

    m_pFont28 = TTF_OpenFont("Retro Gaming.ttf", 28);
    m_pFont40 = TTF_OpenFont("Retro Gaming.ttf", 40);

#ifdef __unix__
    if (!UnixScaling())
        return false;
#endif

    return true;
}

static SDL_Color ConvertHex (uint32_t hex)
{
    SDL_Color color;
    // Bitwise right (>>) cuts off last x number of bits
    // Move enough bits so individual value is rightmost 2 hex digits
    // After, perform an & operation with 255 to chop off the rest
    // e.g. 1111 & 0011 == 0011
    color.r = ((hex >> 24) & 0xFF);
    color.g = ((hex >> 16) & 0xFF);
    color.b = ((hex >> 8) & 0xFF);
    color.a = (hex & 0xFF);
    return color;
}

void GameWindow::Draw (Board* currentBoard)
{
    // int startTime = SDL_GetTicks();
    static uint8_t squareSize = 45;

    static int boardScreenW = Board::WIDTH * squareSize;
    static int boardScreenH = Board::VISIBLE_HEIGHT * squareSize;

    static int boardOffsetX = (WINDOW_W - boardScreenW) / 2;
    static int boardOffsetY = (WINDOW_H - boardScreenH) / 2;

    // offsets are so the blocks don't overlap with the border
    SDL_FRect boardOutline = {
        (float) boardOffsetX - 1,
        (float) boardOffsetY - 1,
        (float) boardScreenW + 2,
        (float) boardScreenH + 2
    };

    SDL_SetRenderDrawColor(m_pRenderer, 18, 18, 18, 255);
    SDL_RenderClear(m_pRenderer);

    SDL_SetRenderDrawColor(m_pRenderer, 93, 93, 93, 255);
    SDL_RenderRect(m_pRenderer, &boardOutline);

    if (currentBoard->GetFallingPiece() > 0)
    {
        DrawGhostPiece
            (
                currentBoard,
                boardOffsetX,
                boardOffsetY,
                currentBoard->GetFallingPiece(),
                currentBoard->GetFallingPieceRot(),
                squareSize
            );
    }

    for (int y = 0; y < Board::VISIBLE_HEIGHT; y++)
    {
        int absy = y * squareSize + boardOffsetY;

        for (int x = 0; x < Board::WIDTH; x++)
        {
            int absx = x * squareSize + boardOffsetX;

            int8_t sq = currentBoard->GetSquare(x, y + OFFSCREEN_ROWS);
            if (sq == 0)
                continue;

            SDL_FRect sqRect = {
                (float) absx,
                (float) absy,
                (float) squareSize,
                (float) squareSize
            };

            unsigned int hex = TetrominoData::HEX_CODES[abs(sq) - 1];
            SDL_Color color = ConvertHex(hex);
            SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b,
                                   color.a);
            SDL_RenderFillRect(m_pRenderer, &sqRect);
        }
    }

    SDL_Color txtColor = {255, 255, 255};

    uint8_t heldPiece = currentBoard->GetHeldPiece();
    if (heldPiece > 0)
    {
        const int heldPieceOffsetX = boardOffsetX - 6 * squareSize;
        const int heldPieceOffsetY = boardOffsetY + 2 * squareSize;
        DrawPiece(heldPieceOffsetX, heldPieceOffsetY, heldPiece, 0, squareSize);
        DrawTxt(heldPieceOffsetX + 2 * squareSize,
                heldPieceOffsetY - squareSize, "HOLD", m_pFont28, txtColor);
    }

    // Draw up next
    const int upNextOffsetX = boardOffsetX + boardScreenW + 2 * squareSize;
    const int upNextOffsetY = boardOffsetY + 2 * squareSize;
    for (int i = 0; i < 3; i++)
    {
        DrawPiece(upNextOffsetX, upNextOffsetY + 3 * i * squareSize,
                  currentBoard->NthPiece(i), 0, squareSize);
    }
    DrawTxt(upNextOffsetX + 2 * squareSize, upNextOffsetY - squareSize,
            "UP NEXT", m_pFont28, txtColor);

    // Draw score
    uint16_t scoreOffsetX = boardOffsetX + boardScreenW + 4*squareSize;
    uint16_t scoreOffsetY = boardOffsetY + 12 * squareSize;
    DrawLabeledNumber
    (
        scoreOffsetX,
        scoreOffsetY,
        squareSize,
        "SCORE",
        currentBoard->GetScore(),
        m_pFont28,
        txtColor
    );
    uint16_t linesOffsetY = scoreOffsetY + squareSize*2 + 20;

    DrawLabeledNumber
    (
        scoreOffsetX,
        linesOffsetY,
        squareSize,
        "LINES",
        currentBoard->GetLinesCleared(),
        m_pFont28,
        txtColor
    );

    // Game Over screen
    if (currentBoard->GameOver())
    {
        /*
        boardOutline =
        {
                boardOffsetX,
                boardOffsetY,
                boardScreenW,
                boardScreenH
        };
        */
        SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 150);
        SDL_RenderFillRect(m_pRenderer, &boardOutline);
        SDL_Color gameOverTxtColor = {217, 59, 59};
        DrawTxt(boardOffsetX + boardScreenW / 2,
                boardOffsetY + boardScreenH / 2 - 20, "GAME OVER", m_pFont40,
                gameOverTxtColor);
        DrawTxt(boardOffsetX + boardScreenW / 2,
                boardOffsetY + boardScreenH / 2 + 20, "Press R to Restart",
                m_pFont28, gameOverTxtColor);
    }

    /*
    // Draw anchor square
    // Use for debugging
    int anchor = currentBoard->GetFallingPieceAnchor() - OFFSCREEN_SQUARES;
    int x = (anchor % 10)*squareSize + boardOffsetX;
    int y = (anchor / 10)*squareSize + boardOffsetY;

    SDL_FRect anchorSq =
    {
            (float) x,
            (float) y,
            (float) squareSize,
            (float) squareSize
    };
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 255, 255);
    SDL_RenderFillRect(m_pRenderer, &anchorSq);
    /* */

    SDL_RenderPresent(m_pRenderer);
}

void GameWindow::DrawPiece (uint16_t x, uint16_t y, uint8_t piece, uint8_t rot, uint8_t sqSize)
{
    if (piece == I_PIECE)
        y -= sqSize / 2;
    else if (piece != O_PIECE)
        x += sqSize / 2;

    for (int i = 0; i < 4; i++)
    {
        int delta = TetrominoData::GetPieceMap(piece, rot, i);
        int r = Board::Row(delta);
        int c = Board::Col(delta);
        SDL_FRect pieceSq = {
            (float) x + c * sqSize,
            (float) y + r * sqSize,
            (float) sqSize,
            (float) sqSize
        };

        unsigned int hex = TetrominoData::HEX_CODES[piece - 1];
        SDL_Color color = ConvertHex(hex);
        SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(m_pRenderer, &pieceSq);
    }
}


void GameWindow::DrawGhostPiece (Board* currentBoard, uint16_t xOffset, uint16_t yOffset, uint8_t piece, uint8_t rot, uint8_t sqSize)
{
    for (int i = 0; i < 4; i++)
    {
        int delta = TetrominoData::GetPieceMap(piece, rot, i);
        int ghostIdx = currentBoard->GetGhost();
        ghostIdx -= OFFSCREEN_SQUARES;
        int r = Board::Row(ghostIdx + delta);
        int c = Board::Col(ghostIdx + delta);
        SDL_FRect pieceSq = {
            (float) c * sqSize + xOffset,
            (float) r * sqSize + yOffset,
            (float) sqSize,
            (float) sqSize
        };

        unsigned int hex = TetrominoData::HEX_CODES[piece - 1];
        SDL_Color color = ConvertHex(hex);
        color.a = 75;
        SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(m_pRenderer, &pieceSq);
    }
}

void GameWindow::DrawLabeledNumber (uint16_t x, uint16_t y, uint8_t offset, const char* label, size_t number, TTF_Font* font, SDL_Color color)
{
    DrawTxt(x, y, label, font, color);
    std::string numString = std::to_string(number);
    char const* numChars = numString.c_str();
    DrawTxt(x, y + offset, numChars, m_pFont28, color);
}

void GameWindow::DrawTxt (uint16_t x, uint16_t y, const char* txt, TTF_Font* font,
                          SDL_Color color)
{
    if (font == nullptr)
    {
        std::cout << "Failed to load font" << std::endl;
        std::cout << "SDL_TTF ERR: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, txt, 0, color);
    if (textSurface == nullptr)
    {
        std::cout << "Failed to initialize text surface" << std::endl;
        return;
    }
    SDL_Texture* textTexture =
        SDL_CreateTextureFromSurface(m_pRenderer, textSurface);
    if (textTexture == nullptr)
    {
        std::cout << "Failed to initialize text texture" << std::endl;
        return;
    }

    SDL_FRect src = {
        0.0, 
        0.0, 
        (float) textSurface->w, 
        (float) textSurface->h
    };
    SDL_FRect dst = {
        (float) x - src.w / 2, 
        (float) y - src.h / 2, 
        (float) src.w, 
        (float) src.h
    };

    SDL_RenderTexture(m_pRenderer, textTexture, &src, &dst);
    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

GameWindow::~GameWindow ()
{
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_pWindow);
    TTF_CloseFont(m_pFont28);
    TTF_CloseFont(m_pFont40);

    TTF_Quit();
    SDL_Quit();
}
