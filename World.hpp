#pragma once

#include "IWorld.hpp"
#include <vector>

class World final : public IWorld {
    size_t m_width, m_height;
    std::vector<Cell> m_active, m_future;

    size_t index(size_t x, size_t y) const;
    Cell get_active(size_t x, size_t y) const;
    void set_future(size_t x, size_t y, Cell cell);

    Cell up_left(Cell* in) const;
    Cell up(Cell* in) const;
    Cell up_right(Cell* in) const;
    Cell down_left(Cell* in) const;
    Cell down(Cell* in) const;
    Cell down_right(Cell* in) const;
    Cell left(Cell* in) const;
    Cell right(Cell* in) const;
    Cell here(Cell* in) const;

public:
    World();
    World(size_t width, size_t height);
    World(IWorld const& world);

    void resize(size_t width, size_t height) override;
    size_t width() const override;
    size_t height() const override;
    Cell get(size_t x, size_t y) const override;
    void set(size_t x, size_t y, Cell cell) override;

    void update();
};

