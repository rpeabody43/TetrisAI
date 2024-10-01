#include <iostream>

#include "ai/genetic/Agent.hpp"
#include "app/App.hpp"

int main (int argc, char* argv[]) {
    App app (
        new Agent (true, {-20.0, -10.0, 50.0, -1.0, -20.0, -10.0})
    );

    if (!app.init()) {
        std::cout << "ERR: Could not initialize" << std::endl;
        return 1;
    }


    app.run();
    return 0;
}
