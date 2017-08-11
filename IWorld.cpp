#include "IWorld.hpp"
#include <ostream>
#include <random>

void IWorld::copy(IWorld const& world) {
    resize(world.width(), world.height());
    for (size_t y = 0; y < height(); ++y)
        for (size_t x = 0; x < width(); ++x)
            set(x, y, world.get(x, y));
}

void IWorld::populate_uniform(double chance, unsigned seed) {
    if (!seed)
        seed = std::random_device{}();
    std::mt19937 gen(seed);
    std::bernoulli_distribution dist(chance);
    for (size_t y = 0; y < height(); ++y)
        for (size_t x = 0; x < width(); ++x)
            set(x, y, to_cell(dist(gen)));
}

bool operator==(IWorld const& lhs, IWorld const& rhs) {
    if (lhs.width() != rhs.width() || lhs.height() != rhs.height())
        return false;

    for (size_t y = 0; y < lhs.height(); ++y)
        for (size_t x = 0; x < lhs.width(); ++x)
            if (lhs.get(x, y) != rhs.get(x, y))
                return false;

    return true;
}

std::ostream& operator<<(std::ostream& os, IWorld const& world) {
    for (size_t y = 0; y < world.height(); ++y) {
        for (size_t x = 0; x < world.width(); ++x)
            os << show_cell(world.get(x, y));
        os << '\n';
    }
    return os;
}

