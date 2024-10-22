#pragma once

#include "Agent.hpp"

// https://www.codingwiththomas.com/blog/c-genetic-algorithm
struct TrainingSettings {
    const uint32_t POPULATION_SIZE;
    const uint8_t PARENT_RATIO;
    const uint8_t MUTATE_PROBABILITY;
    const uint8_t TRANSFER_RATIO;
    const uint8_t CROSSOVER;
};

/**
* @param settings that affect how the algorithm runs
* @return the best Agent after training
*/
Agent train (TrainingSettings settings);
