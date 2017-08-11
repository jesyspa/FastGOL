#pragma once

#include <cstddef>
#include <iosfwd>

enum Cell { dead, alive };

inline Cell to_cell(bool b) {
    return b ? alive : dead;
}

inline char show_cell(Cell cell) {
    return cell == alive ? 'X' : ' ';
}

// Abstract base class for arbitrary world-like classes.
//
// The interface is fairly rich, so that comparison, printing, and copying can be implemented in
// general terms rather than having to be written for every class.
class BaseWorld {
public:
    virtual ~BaseWorld() = default;
    virtual void resize(size_t width, size_t height) = 0;
    virtual size_t width() const = 0;
    virtual size_t height() const = 0;
    virtual Cell get(size_t x, size_t y) const = 0;
    virtual void set(size_t x, size_t y, Cell cell) = 0;

    // Copy the contents of world.
    virtual void copy(BaseWorld const& world);

    // Fill the world with living cells uniformly, with each cell having chance probability to be
    // alive.
    virtual void populate_uniform(double chance, unsigned seed = 0);
};

bool operator==(BaseWorld const& lhs, BaseWorld const& rhs);
std::ostream& operator<<(std::ostream& os, BaseWorld const& world);

