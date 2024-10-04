#include <iostream>

#include "ai/genetic/Agent.hpp"
#include "ai/genetic/train.hpp"
#include "app/App.hpp"

int main (int argc, char** argv) {
    Weights weights = {-20.0, -10.0, 50.0, -1.0, -20.0, -10.0};
    if (strcmp(argv[1], "train") == 0) {
        Agent best_agent = train({
            .POPULATION_SIZE = 500,
            .PARENT_RATIO = 50,
            .MUTATE_PROBABILITY = 10,
            .TRANSFER_RATIO = 15,
            .CROSSOVER = 50,
        });
        weights = best_agent.get_weights();
    }

    App app (
        new Agent (true, weights)
    );

    if (!app.init()) {
        std::cout << "ERR: Could not initialize" << std::endl;
        return 1;
    }

    app.run();
    return 0;
}
