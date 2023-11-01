#include <iostream>
#include <random>
#include <chrono>

#include <SDL.h>

#include "App.h"
#include "game/Board.h"
#include "ai/Agent.h"

constexpr uint16_t WINDOW_W = 1280;
constexpr uint16_t WINDOW_H = 960;

struct KeyHandler
{
    uint16_t scancode;
    uint8_t delay;
    uint8_t firstDelay; // Add more input buffer between the first and second input
    uint8_t currentDelay;
    uint8_t inputCount;
};

App::App ()
    : m_pBoard(nullptr)
      , m_window(WINDOW_W, WINDOW_H)
      , m_randomGenerator(std::chrono::system_clock::now().time_since_epoch().count())
{}

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

static void PersistentKey (const uint8_t* keystate, KeyHandler& k, bool& input)
{
    if (keystate[k.scancode])
    {
        if (k.currentDelay == 0)
        {
            input = true;
            k.inputCount++;
        }
        SDL_Delay(1); // Delay one millisecond for framerate-independence
        k.currentDelay++;
        if (k.inputCount == 1)
        {
            if (k.currentDelay >= k.firstDelay)
                k.currentDelay = 0;
        }
        else if (k.currentDelay >= k.delay)
            k.currentDelay = 0;
    }
    else
    {
        k.currentDelay = 0;
        k.inputCount = 0;
    }
}

Input UserInput (const uint8_t* keystate)
{
    Input input = {};

    static KeyHandler moveLeft = {SDL_SCANCODE_LEFT, 25, 50, 0, 0};
    static KeyHandler moveRight = {SDL_SCANCODE_RIGHT, 25, 50, 0, 0};
    static KeyHandler softDrop = {SDL_SCANCODE_DOWN, 20, 20, 0, 0};
    static KeyHandler rotClockwise = {SDL_SCANCODE_UP, 75, 75, 0, 0};
    static KeyHandler rotCountClockwise = {SDL_SCANCODE_Z, 75, 75, 0, 0};

    PersistentKey(keystate, softDrop, input.softDrop);
    PersistentKey(keystate, moveLeft, input.moveLeft);
    PersistentKey(keystate, moveRight, input.moveRight);
    PersistentKey(keystate, rotClockwise, input.rotClockwise);
    PersistentKey(keystate, rotCountClockwise, input.rotCountClockwise);

    return input;
}

bool FilterAiInput ()
{
    static uint8_t delayMs = 20;
    static uint8_t currentDelay = 0;

    SDL_Delay(1);
    currentDelay++;
    if (currentDelay >= delayMs)
    {
        currentDelay = 0;
        return true;
    }
    return false;
}

void App::Run ()
{
    bool aiPlayer = true;
    Agent agent;

    bool end = false;
    while (!end)
    {
        SDL_Event event;

        Input input {};
        if (aiPlayer)
        {
            if (m_pBoard->GetFallingPiece() > 0)
            {
                if (FilterAiInput())
                {
                    input = agent.MakeMove(m_pBoard);
                }

            }
        }
        else
        {
            const uint8_t* keystate = SDL_GetKeyboardState(nullptr);
            input = UserInput(keystate);
        }

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                end = true;
            }
            if (event.type == SDL_KEYDOWN)
            {
                if (!aiPlayer)
                {
                    if (event.key.keysym.sym == SDLK_c)
                        input.holdPiece = true;
                    if (event.key.keysym.sym == SDLK_SPACE)
                        input.hardDrop = true;
                }
                if (event.key.keysym.sym == SDLK_r)
                {
                    NewGame();
                    continue;
                }
            }
        }

        uint32_t currentTimeMs = SDL_GetTicks();
        m_pBoard->Update(input, currentTimeMs);

        m_window.Draw(m_pBoard);
    }
}

App::~App ()
{
    delete m_pBoard;
}