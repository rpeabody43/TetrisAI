#pragma once

#include "../ai/Player.h"

/* Holds data corresponding to an input */
struct KeyHandler
{
    uint16_t scancode;
    uint8_t delay;
    uint8_t firstDelay; // Add more input buffer between the first and second input
    uint8_t currentDelay;
    uint8_t inputCount;
};

class HumanPlayer : public Player
{
public:
    Input GenInput ([[maybe_unused]] Board* board) override;

private:

    /**
     * Uses SDL to get game input from the keyboard.
     * @param keystate the return of SDL_GetKeyboardState().
     * @return An Input object.
     */
    Input UserInputFromKeys (const bool* keystate);

    /**
     * Prevents a "held" key from firing over and over again
     * @param keystate The return of SDL_GetKeyboardState()
     * @param k Which KeyHandler to use.
     * @return True if the input should be active.
     */
    static bool PersistentKey (const bool* keystate, KeyHandler& k);

    KeyHandler m_moveLeft = {SDL_SCANCODE_LEFT, 25, 50, 0, 0};
    KeyHandler m_moveRight = {SDL_SCANCODE_RIGHT, 25, 50, 0, 0};
    KeyHandler m_softDrop = {SDL_SCANCODE_DOWN, 20, 20, 0, 0};
    KeyHandler m_rotClockwise = {SDL_SCANCODE_UP, 75, 75, 0, 0};
    KeyHandler m_rotCountClockwise = {SDL_SCANCODE_Z, 75, 75, 0, 0};
};
