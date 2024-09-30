#include <SDL3/SDL.h>

#include "HumanPlayer.h"

bool HumanPlayer::PersistentKey (const bool* keystate, KeyHandler& k)
{
    bool ret = false;
    if (keystate[k.scancode])
    {
        if (k.currentDelay == 0)
        {
            ret = true;
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
    return ret;
}

Input HumanPlayer::UserInputFromKeys (const bool* keystate)
{
    Input input =
    {
        .moveLeft           =   PersistentKey(keystate, m_moveLeft),
        .moveRight          =   PersistentKey(keystate, m_moveRight),
        .rotClockwise       =   PersistentKey(keystate, m_rotClockwise),
        .rotCountClockwise  =   PersistentKey(keystate, m_rotCountClockwise),
        .softDrop           =   PersistentKey(keystate, m_softDrop),
    };


    return input;
}

Input HumanPlayer::GenInput([[maybe_unused]] Board* board)
{
    return UserInputFromKeys(SDL_GetKeyboardState(nullptr));
}
