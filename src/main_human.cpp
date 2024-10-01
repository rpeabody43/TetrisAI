#include <iostream>

#include "app/App.hpp"

int main (int argc, char** argv) {
    App app;

    if (!app.init()) {
        std::cout << "ERR: Could not initialize" << std::endl;
        return 1;
    }

    app.run();
    return 0;
}
