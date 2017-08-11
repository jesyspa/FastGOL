#pragma once

#include "IWorld.hpp"
#include <vector>

// Reference implementation for the game of life; slow but steady.
class ReferenceWorld final : public IWorld{
    size_t m_width, m_height;
    std::vector<Cell> m_data;

    bool valid(size_t x, size_t y) const;
    size_t index(size_t x, size_t y) const;

public:
    ReferenceWorld() : m_width(), m_height(), m_data() {}
    ReferenceWorld(size_t width, size_t height);

    void resize(size_t width, size_t height) override;
    size_t width() const override;
    size_t height() const override;
    Cell get(size_t x, size_t y) const override;
    void set(size_t x, size_t y, Cell cell) override;

    void update();
};

