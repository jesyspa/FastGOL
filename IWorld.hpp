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

class IWorld {
public:
    virtual ~IWorld() = default;
    virtual void resize(size_t width, size_t height) = 0;
    virtual size_t width() const = 0;
    virtual size_t height() const = 0;
    virtual Cell get(size_t x, size_t y) const = 0;
    virtual void set(size_t x, size_t y, Cell cell) = 0;

    virtual void copy(IWorld const& world);
    virtual void populate_uniform(double chance, unsigned seed = 0);
};

bool operator==(IWorld const& lhs, IWorld const& rhs);
std::ostream& operator<<(std::ostream& os, IWorld const& world);

