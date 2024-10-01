#pragma once

#include "../ai/Player.hpp"

/* Holds data corresponding to an input */
struct KeyHandler {
    uint16_t scancode;
    uint8_t delay;
    uint8_t first_delay; // Add more input buffer between the first and second input
    uint8_t current_delay;
    uint8_t input_count;
};

class HumanPlayer : public Player {
public:
    Input gen_input ([[maybe_unused]] Board* board) override;

private:

    /**
     * Uses SDL to get game input from the keyboard.
     * @param keystate the return of SDL_GetKeyboardState().
     * @return An Input object.
     */
    Input user_input_from_keys (const bool* keystate);

    /**
     * Prevents a "held" key from firing over and over again
     * @param keystate The return of SDL_GetKeyboardState()
     * @param k Which KeyHandler to use.
     * @return True if the input should be active.
     */
    static bool persistent_key (const bool* keystate, KeyHandler& k);

    KeyHandler m_move_left = {SDL_SCANCODE_LEFT, 25, 50, 0, 0};
    KeyHandler m_move_right = {SDL_SCANCODE_RIGHT, 25, 50, 0, 0};
    KeyHandler m_soft_drop = {SDL_SCANCODE_DOWN, 20, 20, 0, 0};
    KeyHandler m_rot_clockwise = {SDL_SCANCODE_UP, 75, 75, 0, 0};
    KeyHandler m_rot_count_clockwise = {SDL_SCANCODE_Z, 75, 75, 0, 0};
};
