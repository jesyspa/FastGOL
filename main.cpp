#include "World.hpp"
#include <iostream>
#include <chrono>

size_t const WORLD_SIZE = 500;
size_t const ITERATIONS = 1000;

int main() {
    World world(WORLD_SIZE, WORLD_SIZE);
    world.populate_uniform(0.3);
    auto time_start = std::chrono::system_clock::now();
    for (size_t i = 0; i < ITERATIONS; ++i)
        world.update();
    auto time_end = std::chrono::system_clock::now();

    auto duration = time_end - time_start;
    std::cout << "Time: " << (0.000'000'1 * std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / ITERATIONS)
        << " milliseconds per iteration.\n";
}
