#pragma once

#include <vector>
#include <iosfwd>

enum Cell { dead, alive };

inline Cell to_cell(bool b) {
    return b ? alive : dead;
}

class World {
    size_t width, height;
    std::vector<Cell> active, future;

    size_t index(size_t x, size_t y) const;
    Cell get_active(size_t x, size_t y) const;
    void set_future(size_t x, size_t y, Cell cell);

    size_t up(size_t p) const;
    size_t down(size_t p) const;
    size_t left(size_t p) const;
    size_t right(size_t p) const;

public:
    World(size_t width, size_t height);

    void populate_uniform(double chance, unsigned seed = 0);
    void update();

    friend std::ostream& operator<<(std::ostream& os, World const& world);
};

