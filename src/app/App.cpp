#include <iostream>
#include <random>
#include <chrono>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "App.h"
#include "HumanPlayer.h"
#include "../game/Board.h"

App::App (Player* player)
    : m_pBoard(nullptr)
      , m_window()
      , m_randomGenerator(std::chrono::system_clock::now().time_since_epoch().count())
      , m_userInput(false)
      , m_pPlayer(player)
{}

App::App () : App(new HumanPlayer())
{
    m_userInput = true;
}

bool App::Init ()
{
    NewGame();
    return m_window.Init();
}

void App::NewGame ()
{
    delete m_pBoard;
    m_pBoard = new Board(250, m_randomGenerator);
}

/**
 * Prevents automated input from going at insane speeds
 * @return True if input should be generated.
 */
bool FilterAIInput ()
{
    static const uint8_t delayMs = 20;
    static uint8_t currentDelay = 0;

    SDL_Delay(1);
    currentDelay++;
    if (currentDelay >= delayMs)
    {
        currentDelay = 0;
    }
    return currentDelay == 0;
}


void App::Run ()
{
    bool end = false;
    while (!end)
    {
        SDL_Event event;

        Input input = {};
        if (m_userInput || FilterAIInput())
        {
            input = m_pPlayer->GenInput(m_pBoard);
        }

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                end = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (m_userInput)
                {
                    if (event.key.key == SDLK_C)
                        input.holdPiece = true;
                    if (event.key.key == SDLK_SPACE)
                        input.hardDrop = true;
                }
                if (event.key.key == SDLK_R)
                {
                    NewGame();
                    continue;
                }
            }
        }

        uint64_t currentTimeMs = SDL_GetTicks();
        m_pBoard->Update(input, currentTimeMs);

        m_window.Draw(m_pBoard);
    }
}

App::~App ()
{
    delete m_pBoard;
    delete m_pPlayer;
}
