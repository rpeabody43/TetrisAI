#include <SDL3/SDL.h>

#include "HumanPlayer.hpp"

bool HumanPlayer::persistent_key (const bool* keystate, KeyHandler& k) {
    bool ret = false;
    if (keystate[k.scancode])
    {
        if (k.current_delay == 0)
        {
            ret = true;
            k.input_count++;
        }
        SDL_Delay(1); // Delay one millisecond for framerate-independence
        k.current_delay++;
        if (k.input_count == 1)
        {
            if (k.current_delay >= k.first_delay)
                k.current_delay = 0;
        }
        else if (k.current_delay >= k.delay)
            k.current_delay = 0;
    }
    else
    {
        k.current_delay = 0;
        k.input_count = 0;
    }
    return ret;
}

Input HumanPlayer::user_input_from_keys (const bool* keystate) {
    Input input = {
        .move_left           = persistent_key(keystate, m_move_left),
        .move_right          = persistent_key(keystate, m_move_right),
        .rot_clockwise       = persistent_key(keystate, m_rot_clockwise),
        .rot_count_clockwise = persistent_key(keystate, m_rot_count_clockwise),
        .soft_drop           = persistent_key(keystate, m_soft_drop),
    };


    return input;
}

Input HumanPlayer::gen_input([[maybe_unused]] Board* board) {
    return user_input_from_keys(SDL_GetKeyboardState(nullptr));
}
