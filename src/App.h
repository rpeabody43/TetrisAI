#pragma once

#include <random>

#include <SDL.h>
#include <SDL_ttf.h>

#include "game/Board.h"
#include "gfx/Window.h"

/* Controls the game loop. */
class App
{
public:
    /**
     * Creates a new App. 
     * Run App.Init() afterwards.
     * @param screenW The width of the window.
     * @param screenH The height of the window.
     */
    App ();

    /* Resets the game state. */
    void NewGame ();

    /**
     * Initializes the Window part of the App.
     * @return True if everything goes well, false otherwise.
     */
    bool Init ();

    /**
     * Starts running the game.
     * Only exits when the game is closed by the user.
     */
    void Run ();

    /**
     * Destructor: cleans stuff up.
     */
    ~App ();

private:

    std::default_random_engine m_randomGenerator;
    Board* m_pBoard;
    GameWindow m_window;
};