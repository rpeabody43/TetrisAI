#pragma once

#include <functional>
#include <random>

#include <SDL.h>

#include "game/Board.h"
#include "gfx/Window.h"
#include "ai/Player.h"

/* Controls the game loop. */
class App
{
public:
    /**
     * Creates a new App with user input.
     * For when the game is being played by a human.
     * Run App.Init() afterwards.
     */
    App ();

    /**
     * Creates a new App with a certain kind of player.
     * @param player An object that can generate moves from the board state.
     *
     * APP TAKES CONTROL OF AND DELETES PLAYER IN DESTRUCTOR.
     */
    explicit App (Player* player);

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
    bool m_userInput;
    Player* m_pPlayer;
};