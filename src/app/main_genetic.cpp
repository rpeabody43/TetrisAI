#include <iostream>

#include "ai/genetic/Agent.h"
#include "App.h"

int main (int argc, char* argv[])
{
    App app
    (
        new Agent (false, {-20.0, -10.0, 50.0, -1.0, -20.0, -10.0})
    );

    if (!app.Init())
    {
        std::cout << "ERR: Could not initialize" << std::endl;
        return 1;
    }


    app.Run();
    return 0;
}