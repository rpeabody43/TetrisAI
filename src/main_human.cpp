#include <iostream>

#include "app/App.h"

int main (int argc, char* argv[])
{
    App app;

    if (!app.Init())
    {
        std::cout << "ERR: Could not initialize" << std::endl;
        return 1;
    }

    app.Run();
    return 0;
}
