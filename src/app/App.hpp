#pragma once

#include <functional>
#include <random>

#include <SDL3/SDL.h>

#include "../game/Board.hpp"
#include "../ai/Player.hpp"
#include "gfx/Window.hpp"

/* Controls the game loop. */
class App {
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
     *
     * Run App.Init() afterwards.
     */
    explicit App (Player* player);

    /* Resets the game state. */
    void new_game ();

    /**
     * Initializes the Window part of the App.
     * @return True if everything goes well, false otherwise.
     */
    bool init ();

    /**
     * Starts running the game.
     * Only exits when the game is closed by the user.
     */
    void run ();

    /**
     * Destructor: cleans stuff up.
     */
    ~App ();

private:

    std::default_random_engine m_randomgen;
    Board* m_board;
    GameWindow m_window;
    bool m_user_input;
    Player* m_player;
};
