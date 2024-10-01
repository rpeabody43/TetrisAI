#include <iostream>
#include <random>
#include <chrono>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "App.hpp"
#include "HumanPlayer.hpp"
#include "../game/Board.hpp"

App::App (Player* player)
    : m_board(nullptr)
    , m_window()
    , m_randomgen(std::chrono::system_clock::now().time_since_epoch().count())
    , m_user_input(false)
    , m_player(player) 
{}

App::App () : App(new HumanPlayer()) {
    m_user_input = true;
}

bool App::init () {
    new_game();
    return m_window.init();
}

void App::new_game () {
    delete m_board;
    m_board = new Board(250, m_randomgen);
}

/**
 * Prevents automated input from going at insane speeds
 * @return True if input should be generated.
 */
bool filter_ai_input () {
    static const uint8_t delay_ms = 20;
    static uint8_t current_delay = 0;

    SDL_Delay(1);
    current_delay++;
    if (current_delay >= delay_ms) {
        current_delay = 0;
    }
    return current_delay == 0;
}


void App::run () {
    bool end = false;
    while (!end) {
        SDL_Event event;

        Input input = {};
        if (m_user_input || filter_ai_input()) {
            input = m_player->gen_input(m_board);
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                end = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (m_user_input) {
                    if (event.key.key == SDLK_C)
                        input.hold_piece = true;
                    if (event.key.key == SDLK_SPACE)
                        input.hard_drop = true;
                }
                if (event.key.key == SDLK_R) {
                    new_game();
                    continue;
                }
            }
        }

        uint64_t current_time_ms = SDL_GetTicks();
        m_board->update(input, current_time_ms);

        m_window.draw(m_board);
    }
}

App::~App () {
    delete m_board;
    delete m_player;
}
