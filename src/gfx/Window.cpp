#include <iostream>

#include <SDL.h>
#include <SDL_ttf.h>

#if defined(_WIN32) || defined(WIN32)

#include <Windows.h> // To disable Windows scaling

#define OS_WINDOWS

#endif

#include "Window.h"

GameWindow::GameWindow (uint16_t screenW, uint16_t screenH)
    : m_pRenderer(nullptr)
      , m_pWindow(nullptr)
      , m_pFont28(nullptr)
      , m_pFont40(nullptr)
      , m_screenW(screenW)
      , m_screenH(screenH)
{}

bool GameWindow::UnixScaling ()
{
    int rw = 0, rh = 0;
    SDL_GetRendererOutputSize(m_pRenderer, &rw, &rh);

    std::cout << "rw: " << rw << ", rh: " << rh << std::endl;

    if (rw != m_screenW)
    {
        float wScale = (float) rw / (float) m_screenW;
        float hScale = (float) rh / (float) m_screenH;

        if (wScale != hScale)
        {
            std::cout << "SCALING ERROR: Width scale != Height scale"
                      << std::endl;
            return false;
        }

        SDL_RenderSetScale(m_pRenderer, wScale, hScale);
    }
    return true;
}

bool GameWindow::Init ()
{

    if (SDL_Init(SDL_INIT_VIDEO) > 0)
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
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            m_screenW,
            m_screenH,
            SDL_WINDOW_ALLOW_HIGHDPI
        );

    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_SOFTWARE);
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

    static int boardOffsetX = (m_screenW - boardScreenW) / 2;
    static int boardOffsetY = (m_screenH - boardScreenH) / 2;

    // offsets are so the blocks don't overlap with the border
    SDL_Rect boardOutline = {boardOffsetX - 1, boardOffsetY - 1,
                             boardScreenW + 2, boardScreenH + 2};

    SDL_SetRenderDrawColor(m_pRenderer, 18, 18, 18, 255);
    SDL_RenderClear(m_pRenderer);

    SDL_SetRenderDrawColor(m_pRenderer, 93, 93, 93, 255);
    SDL_RenderDrawRect(m_pRenderer, &boardOutline);

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

            int8_t sq = currentBoard->GetSquare(x, y + Board::VANISH_ZONE_HEIGHT);
            if (sq == 0)
                continue;

            SDL_Rect sqRect = {absx, absy, squareSize, squareSize};

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

    const int upNextOffsetX = boardOffsetX + boardScreenW + 2 * squareSize;
    const int upNextOffsetY = boardOffsetY + 2 * squareSize;
    for (int i = 0; i < 3; i++)
    {
        DrawPiece(upNextOffsetX, upNextOffsetY + 3 * i * squareSize,
                  currentBoard->NthPiece(i), 0, squareSize);
    }
    DrawTxt(upNextOffsetX + 2 * squareSize, upNextOffsetY - squareSize,
            "UP NEXT", m_pFont28, txtColor);

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

    /*int anchor = currentBoard->FallingPieceAnchor();
    int x = (anchor % 10)*squareSize + boardOffsetX;
    int y = (anchor / 10)*squareSize + boardOffsetY;

    SDL_Rect anchorSq =
    {
            x,
            y,
            squareSize,
            squareSize
    };

    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 255, 255);
    SDL_RenderFillRect(m_pRenderer, &anchorSq);*/

    SDL_RenderPresent(m_pRenderer);
}

void GameWindow::DrawPiece (uint16_t x, uint16_t y, uint8_t piece, uint8_t rot, uint8_t sqSize)
{
    if (piece == TetrominoData::I)
        y -= sqSize / 2;
    else if (piece != TetrominoData::O)
        x += sqSize / 2;

    for (int i = 0; i < 4; i++)
    {
        int delta = TetrominoData::GetPieceMap(piece, rot, i);
        int r = Board::Row(delta);
        int c = Board::Col(delta);
        SDL_Rect pieceSq = {x + c * sqSize, y + r * sqSize, sqSize, sqSize};

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
        ghostIdx -= Board::VANISH_ZONE_HEIGHT * Board::WIDTH;
        int r = Board::Row(ghostIdx + delta);
        int c = Board::Col(ghostIdx + delta);
        SDL_Rect pieceSq = {c * sqSize + xOffset, r * sqSize + yOffset, sqSize, sqSize};

        unsigned int hex = TetrominoData::HEX_CODES[piece - 1];
        SDL_Color color = ConvertHex(hex);
        color.a = 75;
        SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(m_pRenderer, &pieceSq);
    }
}

void GameWindow::DrawTxt (int x, int y, const char* txt, TTF_Font* font,
                          SDL_Color color)
{
    if (font == nullptr)
    {
        std::cout << "Failed to load font" << std::endl;
        std::cout << "SDL_TTF ERR: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, txt, color);
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

    SDL_Rect src = {0, 0, textSurface->w, textSurface->h};
    SDL_Rect dst = {x - src.w / 2, y - src.h / 2, src.w, src.h};

    SDL_RenderCopy(m_pRenderer, textTexture, &src, &dst);
    SDL_FreeSurface(textSurface);
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