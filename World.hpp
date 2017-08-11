#pragma once

#include "BaseWorld.hpp"
#include <vector>
#include <cstdint>

// Efficient implementation of GOL.
class World final : public BaseWorld {
public:
    // We pack together several cells into a single object of element_type.
    // The following constants describe how this packing is done.
    //
    // The values are stored little-endian: the leftmost cell is in the least significant bits.
    using element_type = uint64_t;
    static constexpr size_t bits_per_cell = 4;
    static_assert(bits_per_cell == 4, "Other values currently not supported.");
    static constexpr size_t cells_per_element = sizeof(element_type) * 8 / bits_per_cell;
    // How far we need to shift in order to reach the last element.
    static constexpr size_t max_shift = bits_per_cell * (cells_per_element - 1);
    // What mask to use to get a single element.
    static constexpr element_type value_mask = (element_type(1) << bits_per_cell) - 1;
private:
    // Width and height of the world data.
    // This does not include the extra rows above and below the data, since those are outside
    // the world.
    size_t m_width, m_height;
    // m_active stores the current state of the world.
    // m_future is used to store the new data when we perform an update.
    // Strictly speaking, m_future could be local to update, but this would require
    // considerably more dynamic allocation.
    std::vector<element_type> m_active, m_future;

    // Compute the index where cell (x, y) is stored.
    // Note that other cells are stored there, too.
    size_t index(size_t x, size_t y) const;

    // Test whether the given dimensions are allowed, and throw if not.
    void test_size(size_t width, size_t height) const;

public:
    World();
    World(size_t width, size_t height);
    World(BaseWorld const& world);

    void resize(size_t width, size_t height) override;
    size_t width() const override;
    size_t height() const override;
    Cell get(size_t x, size_t y) const override;
    void set(size_t x, size_t y, Cell cell) override;

    void update();
};

