#include "World.hpp"
#include "ReferenceWorld.hpp"
#include <iostream>
#include <chrono>
#include <stdexcept>

size_t const WORLD_SIZE = 1024;
size_t const ITERATIONS = 2000;
double const PROC_SPEED = 3.6; // cycles / nanosecond

size_t const TEST_SIZE = 128;
size_t const TEST_COUNT = 5;

void test_world() {
    ReferenceWorld world(TEST_SIZE, TEST_SIZE);
    world.populate_uniform(0.3);
    ReferenceWorld old(world);
    World real(world);

    world.update();
    real.update();
    if (world == real)
        return;

    std::cerr << "Error!\nWorld mismatch:\n";
    std::cerr << "Initial:\n" << old;
    std::cerr << "Expected:\n" << world;
    std::cerr << "Actual:\n" << real;
    throw std::runtime_error{"Test suite failed."};
}

int main() try {
    std::cout << "Testing... " << std::flush;
    for (size_t i = 0; i < TEST_COUNT; ++i)
        test_world();
    std::cout << "Done!\n";

    std::cout << "Running... " << std::flush;
    World world(WORLD_SIZE, WORLD_SIZE);
    world.populate_uniform(0.3);
    auto time_start = std::chrono::system_clock::now();
    for (size_t i = 0; i < ITERATIONS; ++i)
        world.update();
    auto time_end = std::chrono::system_clock::now();
    std::cout << "Done!\n";

    auto duration = time_end - time_start;
    std::cout << "Time: " << (std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / 1000. / 1000. / ITERATIONS)
        << " milliseconds per iteration.\n";
    std::cout << "Equivalently: " << (std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() * PROC_SPEED / ITERATIONS / WORLD_SIZE / WORLD_SIZE)
        << " clock cycles per cell.\n";
} catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return -1;
} catch (...) {
    std::cerr << "Unknown error.\n";
    return -1;
}
