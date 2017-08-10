#include "World.hpp"
#include <random>
#include <ostream>
#include <algorithm>

namespace { 
    size_t const row_size = 16;
    Cell live_data[row_size * 2];
    void init_live_data() {
        std::fill(std::begin(live_data), std::end(live_data), dead);
        live_data[3] = alive;
        live_data[row_size + 3] = alive;
        live_data[row_size + 4] = alive;
    }
}

World::World(size_t width, size_t height)
    : width(width), height(height),
      active(width*height), future(active)
{
    static int dummy = (init_live_data(), 0);
    (void)dummy;
}

void World::populate_uniform(double chance, unsigned seed) {
    if (!seed)
        seed = std::random_device{}();
    std::mt19937 gen(seed);
    std::bernoulli_distribution dist(chance);
    for (auto& e : active)
        e = to_cell(dist(gen));
}

void World::update() {
    // cell itself always counts as a neighbour
    int neighbours = 0;
    Cell* in = active.data();
    Cell* out = future.data();
    auto click = [&in, &out, &neighbours, this]() {
        *out = live_data[row_size * (int)*in + neighbours];
    };
    auto next = [&in, &out, this]() {
        in = right(in);
        out = right(out);
    };

    // cell 0, 0
    neighbours = *in + *down(in) + *right(in) + *down(right(in));
    click();
    for (size_t i = 0; i < width - 2; ++i) {
        // cell i+1, 0
        next();
        neighbours += *right(in) + *right(down(in));
        click();
        neighbours -= *left(in) + *left(down(in));
    }
    // cell width-1, 0
    next();
    click();

    // looparound
    next();

    for (size_t j = 0; j < height - 2; ++j) {
        // cell 0, j+1
        neighbours = *in + *up(in) + *up(right(in)) + *right(in) + *down(in) + *down(right(in));
        click();

        for (size_t i = 0; i < width - 2; ++i) {
            // cell i+1, j+1
            next();
            neighbours += *right(up(in)) + *right(in) + *right(down(in));
            click();
            neighbours -= *left(up(in)) + *left(in) + *left(down(in));
        }

        // cell width-1, j+1
        next();
        click();

        // looparound
        next();
        neighbours = 0;
    }

    // cell 0, height-1
    neighbours = *in + *up(in) + *right(in) + *up(right(in));
    click();
    for (size_t i = 0; i < width - 2; ++i) {
        // cell i+1, height-1
        next();
        neighbours += *right(in) + *right(up(in));
        click();
        neighbours -= *left(in) + *left(up(in));
    }
    // cell width-1, height-1
    next();
    click();

    active.swap(future);
}

std::ostream& operator<<(std::ostream& os, World const& world) {
    for (size_t y = 0; y < world.height; ++y) {
        for (size_t x = 0; x < world.width; ++x)
            os << (world.get_active(x, y) ? 'X' : ' ');
        os << '\n';
    }
    return os;
}

Cell* World::up(Cell* p) const { return p-width; }
Cell* World::down(Cell* p) const { return p+width; }
Cell* World::left(Cell* p) const { return p-1; }
Cell* World::right(Cell* p) const { return p+1; }

size_t World::index(size_t x, size_t y) const {
    return y * width + x;
}

Cell World::get_active(size_t x, size_t y) const {
    return active[index(x, y)];
}

void World::set_future(size_t x, size_t y, Cell cell) {
    future[index(x, y)] = cell;
}

