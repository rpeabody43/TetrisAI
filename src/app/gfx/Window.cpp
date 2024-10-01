#include <iostream>
#include <string>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#if defined(_WIN32) || defined(WIN32)

#include <windows.h> // To disable Windows scaling

#define OS_WINDOWS

#endif

#include "Window.hpp"


constexpr uint16_t WINDOW_W = 1280;
constexpr uint16_t WINDOW_H = 960;


GameWindow::GameWindow ()
    : m_renderer(nullptr)
    , m_window(nullptr)
    , m_font28(nullptr)
    , m_font40(nullptr)
{}

bool GameWindow::unix_scaling () {
    int rw = 0, rh = 0;
    SDL_GetCurrentRenderOutputSize(m_renderer, &rw, &rh);

    std::cout << "rw: " << rw << ", rh: " << rh << std::endl;

    if (rw != WINDOW_W) {
        float w_scale = (float) rw / (float) WINDOW_W;
        float h_scale = (float) rh / (float) WINDOW_H;

        if (w_scale != h_scale) {
            std::cout << "SCALING ERROR: Width scale != Height scale"
                      << std::endl;
            return false;
        }

        SDL_SetRenderScale(m_renderer, w_scale, h_scale);
    }
    return true;
}

bool GameWindow::init () {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL FAILED TO INITIALIZE: " << SDL_GetError()
                  << std::endl;
        return false;
    }

    TTF_Init();

    // Fix for non-native scaling/DPI (150% in Windows, etc.)
#ifdef OS_WINDOWS
    SetProcessDPIAware();
#endif

    m_window = SDL_CreateWindow(
        "TetrisAI",
        WINDOW_W,
        WINDOW_H,
        SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    m_renderer = SDL_CreateRenderer(m_window, nullptr);
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    m_font28 = TTF_OpenFont("Retro Gaming.ttf", 28);
    m_font40 = TTF_OpenFont("Retro Gaming.ttf", 40);

#ifdef __unix__
    if (!unix_scaling())
        return false;
#endif

    return true;
}

static SDL_Color convert_hex (uint32_t hex) {
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

void GameWindow::draw (Board* current_board) {
    // int startTime = SDL_GetTicks();
    static uint8_t square_size = 45;

    static int board_screen_w = Board::WIDTH * square_size;
    static int board_screen_h = Board::VISIBLE_HEIGHT * square_size;

    static int board_offset_x = (WINDOW_W - board_screen_w) / 2;
    static int board_offset_y = (WINDOW_H - board_screen_h) / 2;

    // offsets are so the blocks don't overlap with the border
    SDL_FRect board_outline = {
        (float) board_offset_x - 1,
        (float) board_offset_y - 1,
        (float) board_screen_w + 2,
        (float) board_screen_h + 2
    };

    SDL_SetRenderDrawColor(m_renderer, 18, 18, 18, 255);
    SDL_RenderClear(m_renderer);

    SDL_SetRenderDrawColor(m_renderer, 93, 93, 93, 255);
    SDL_RenderRect(m_renderer, &board_outline);

    if (current_board->get_falling_piece() > 0) {
        draw_ghost_piece(
            current_board,
            board_offset_x,
            board_offset_y,
            current_board->get_falling_piece(),
            current_board->get_falling_piece_rot(),
            square_size
        );
    }

    for (int y = 0; y < Board::VISIBLE_HEIGHT; y++) {
        int absy = y * square_size + board_offset_y;

        for (int x = 0; x < Board::WIDTH; x++) {
            int absx = x * square_size + board_offset_x;

            int8_t sq = current_board->get_square(x, y + OFFSCREEN_ROWS);
            if (sq == 0)
                continue;

            SDL_FRect sqRect = {
                (float) absx,
                (float) absy,
                (float) square_size,
                (float) square_size
            };

            unsigned int hex = tetromino_data::HEX_CODES[abs(sq) - 1];
            SDL_Color color = convert_hex(hex);
            SDL_SetRenderDrawColor(
                m_renderer, 
                color.r, color.g, color.b,
                color.a
            );
            SDL_RenderFillRect(m_renderer, &sqRect);
        }
    }

    SDL_Color txt_color = {255, 255, 255};

    uint8_t held_piece = current_board->get_held_piece();
    if (held_piece > 0) {
        const int held_piece_offset_x = board_offset_x - 6 * square_size;
        const int held_piece_offset_y = board_offset_y + 2 * square_size;
        draw_piece(
            held_piece_offset_x, 
            held_piece_offset_y,
            held_piece,
            0,
            square_size
        );
        draw_txt(
            held_piece_offset_x + 2 * square_size,
            held_piece_offset_y - square_size,
            "HOLD",
            m_font28,
            txt_color
        );
    }

    // Draw up next
    const int up_next_offset_x = board_offset_x + board_screen_w + 2 * square_size;
    const int up_next_offset_y = board_offset_y + 2 * square_size;
    for (int i = 0; i < 3; i++) {
        draw_piece(
            up_next_offset_x, 
            up_next_offset_y + 3 * i * square_size,
            current_board->nth_piece(i),
            0,
            square_size
        );
    }
    draw_txt(
        up_next_offset_x + 2 * square_size,
        up_next_offset_y - square_size,
        "UP NEXT",
        m_font28, 
        txt_color
    );

    // Draw score
    uint16_t score_offset_x = board_offset_x + board_screen_w + 4*square_size;
    uint16_t score_offset_y = board_offset_y + 12 * square_size;
    draw_labeled_number(
        score_offset_x,
        score_offset_y,
        square_size,
        "SCORE",
        current_board->get_score(),
        m_font28,
        txt_color
    );
    uint16_t lines_offset_y = score_offset_y + square_size*2 + 20;

    draw_labeled_number(
        score_offset_x,
        lines_offset_y,
        square_size,
        "LINES",
        current_board->get_lines_cleared(),
        m_font28,
        txt_color
    );

    // Game Over screen
    if (current_board->game_over()) {
        /*
        board_outline =
        {
            board_offset_x,
            board_offset_y,
            board_screen_w,
            board_screen_h
        };
        */
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 150);
        SDL_RenderFillRect(m_renderer, &board_outline);
        SDL_Color gameover_txt_color = {217, 59, 59};
        draw_txt(
            board_offset_x + board_screen_w / 2,
            board_offset_y + board_screen_h / 2 - 20,
            "GAME OVER", 
            m_font40,
            gameover_txt_color
        );
        draw_txt(
            board_offset_x + board_screen_w / 2,
            board_offset_y + board_screen_h / 2 + 20,
            "Press R to Restart",
            m_font28, 
            gameover_txt_color
        );
    }

    /*
    // Draw anchor square
    // Use for debugging
    int anchor = current_board->get_falling_piece_anchor() - OFFSCREEN_SQUARES;
    int x = (anchor % 10)*square_size + board_offset_x;
    int y = (anchor / 10)*square_size + board_offset_y;

    SDL_FRect anchor_sq =
    {
        (float) x,
        (float) y,
        (float) square_size,
        (float) square_size
    };
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(m_renderer, &anchor_sq);
    /* */

    SDL_RenderPresent(m_renderer);
}

void GameWindow::draw_piece (
    uint16_t x, uint16_t y, uint8_t piece, uint8_t rot, uint8_t sq_size
) {
    if (piece == I_PIECE)
        y -= sq_size / 2;
    else if (piece != O_PIECE)
        x += sq_size / 2;

    for (int i = 0; i < 4; i++) {
        int delta = tetromino_data::get_piece_map(piece, rot, i);
        int r = Board::row(delta);
        int c = Board::col(delta);
        SDL_FRect piece_sq = {
            (float) x + c * sq_size,
            (float) y + r * sq_size,
            (float) sq_size,
            (float) sq_size
        };

        unsigned int hex = tetromino_data::HEX_CODES[piece - 1];
        SDL_Color color = convert_hex(hex);
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(m_renderer, &piece_sq);
    }
}


void GameWindow::draw_ghost_piece (
    Board* current_board, uint16_t x_offset, uint16_t y_offset, 
    uint8_t piece, uint8_t rot, uint8_t sq_size
) {
    for (int i = 0; i < 4; i++)
    {
        int delta = tetromino_data::get_piece_map(piece, rot, i);
        int ghost_idx = current_board->get_ghost();
        ghost_idx -= OFFSCREEN_SQUARES;
        int r = Board::row(ghost_idx + delta);
        int c = Board::col(ghost_idx + delta);
        SDL_FRect piece_sq = {
            (float) c * sq_size + x_offset,
            (float) r * sq_size + y_offset,
            (float) sq_size,
            (float) sq_size
        };

        unsigned int hex = tetromino_data::HEX_CODES[piece - 1];
        SDL_Color color = convert_hex(hex);
        color.a = 75;
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(m_renderer, &piece_sq);
    }
}

void GameWindow::draw_labeled_number (
    uint16_t x, uint16_t y, uint8_t offset, const char* label, 
    size_t number, TTF_Font* font, SDL_Color color
) {
    draw_txt(x, y, label, font, color);
    std::string num_string = std::to_string(number);
    char const* num_chars = num_string.c_str();
    draw_txt(x, y + offset, num_chars, m_font28, color);
}

void GameWindow::draw_txt (
    uint16_t x, uint16_t y, const char* txt,TTF_Font* font, SDL_Color color
) {
    if (font == nullptr) {
        std::cout << "Failed to load font" << std::endl;
        std::cout << "SDL_TTF ERR: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_Surface* text_surface = TTF_RenderText_Blended(font, txt, 0, color);

    if (text_surface == nullptr) {
        std::cout << "Failed to initialize text surface" << std::endl;
        return;
    }

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(
        m_renderer, 
        text_surface
    );

    if (text_texture == nullptr) {
        std::cout << "Failed to initialize text texture" << std::endl;
        return;
    }

    SDL_FRect src = {
        0.0, 
        0.0, 
        (float) text_surface->w, 
        (float) text_surface->h
    };
    SDL_FRect dst = {
        (float) x - src.w / 2, 
        (float) y - src.h / 2, 
        (float) src.w, 
        (float) src.h
    };

    SDL_RenderTexture(m_renderer, text_texture, &src, &dst);
    SDL_DestroySurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

GameWindow::~GameWindow () {
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    TTF_CloseFont(m_font28);
    TTF_CloseFont(m_font40);

    TTF_Quit();
    SDL_Quit();
}
