#include "World.hpp"
#include <random>
#include <ostream>

World::World(size_t width, size_t height)
    : width(width), height(height),
      active(width*height), future(active)
{}

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
    size_t p = 0;
    auto click = [&p, &neighbours, this]() {
        if (active[p])
            future[p] = to_cell(neighbours > 2 && neighbours < 5);
        else 
            future[p] = to_cell(neighbours == 3);
    };

    // cell 0, 0
    neighbours = active[p] + active[down(p)] + active[right(p)] + active[down(right(p))];
    click();
    for (size_t i = 0; i < width - 2; ++i) {
        // cell i+1, 0
        p = right(p);
        neighbours += active[right(p)] + active[right(down(p))];
        click();
        neighbours -= active[left(p)] + active[left(down(p))];
    }
    // cell width-1, 0
    p = right(p);
    click();

    // looparound
    p = right(p);

    for (size_t j = 0; j < height - 2; ++j) {
        // cell 0, j+1
        neighbours = active[p] + active[up(p)] + active[up(right(p))] + active[right(p)] + active[down(p)] + active[down(right(p))];
        click();

        for (size_t i = 0; i < width - 2; ++i) {
            // cell i+1, j+1
            p = right(p);
            neighbours += active[right(up(p))] + active[right(p)] + active[right(down(p))];
            click();
            neighbours -= active[left(up(p))] + active[left(p)] + active[left(down(p))];
        }

        // cell width-1, j+1
        p = right(p);
        click();

        // looparound
        p = right(p);
        neighbours = 0;
    }

    // cell 0, height-1
    neighbours = active[p] + active[up(p)] + active[right(p)] + active[up(right(p))];
    click();
    for (size_t i = 0; i < width - 2; ++i) {
        // cell i+1, height-1
        p = right(p);
        neighbours += active[right(p)] + active[right(up(p))];
        click();
        neighbours -= active[left(p)] + active[left(up(p))];
    }
    // cell width-1, height-1
    p = right(p);
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

size_t World::up(size_t p) const { return p-width; }
size_t World::down(size_t p) const { return p+width; }
size_t World::left(size_t p) const { return p-1; }
size_t World::right(size_t p) const { return p+1; }

size_t World::index(size_t x, size_t y) const {
    return y * width + x;
}

Cell World::get_active(size_t x, size_t y) const {
    return active[index(x, y)];
}

void World::set_future(size_t x, size_t y, Cell cell) {
    future[index(x, y)] = cell;
}

