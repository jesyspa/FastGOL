#pragma once

#include "IWorld.hpp"
#include <vector>
#include <cstdint>

class World final : public IWorld {
public:
    static constexpr size_t cells_per_element = 16;
    using element_type = uint32_t;
private:
    size_t m_width, m_height;
    std::vector<element_type> m_active, m_future;

    size_t index(size_t x, size_t y) const;

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

