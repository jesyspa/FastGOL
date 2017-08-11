#include "World.hpp"
#include <algorithm>
#include <stdexcept>

namespace {
    World::element_type live_data[1 << World::bits_per_cell];
    void init_live_data_impl() {
        live_data[3] = 1;
        live_data[11] = 1;
        live_data[12] = 1;
    }

    void init_live_data() {
        static int dummy = (init_live_data_impl(), 0);
        (void)dummy;
    }
}

World::World() : m_width(), m_height(), m_active(), m_future()
{
    init_live_data();
}

World::World(size_t width, size_t height)
    : m_width(width/cells_per_element), m_height(height),
      m_active(m_width*(m_height+2)), m_future(m_active)
{
    if (width % cells_per_element != 0)
        throw std::runtime_error{"Width must be a multiple of cells_per_element."};
    init_live_data();
}

World::World(IWorld const& world) : World() {
    init_live_data();
    copy(world);
}

void World::update() {
    element_type* in = m_active.data() + m_width;
    element_type* out = m_future.data() + m_width;
    element_type old_neighbours = 0;
    element_type neighbours = 0;
    element_type new_neighbours = 0;

    auto click = [&] {
        element_type in_value = *in;
        for (size_t shift = 1; shift < bits_per_cell; ++shift)
            in_value |= in_value << 1;
        element_type total_neighbours = (old_neighbours >> max_shift)
                + (neighbours << bits_per_cell)
                + neighbours
                + (neighbours >> bits_per_cell)
                + (new_neighbours << max_shift);

        element_type result = total_neighbours ^ in_value;
        element_type option_a = ~(result >> 3) & ~(result >> 2) & (result >> 1) & result;
        element_type option_b = (result >> 3) & ~(result >> 2) & (result >> 1) & result;
        element_type option_c = (result >> 3) & (result >> 2) & ~(result >> 1) & ~result;
        *out = (option_a | option_b | option_c) & 0x1111111111111111ull;
        ++in;
        ++out;
        old_neighbours = neighbours;
        neighbours = new_neighbours;
    };


    for (size_t j = 0; j < m_height; ++j) {
        auto f = [&](int i) { return in[-m_width+i] + in[i] + in[m_width+i]; };
        old_neighbours = 0;
        neighbours = f(0);
        new_neighbours = f(1);
        click();

        for (size_t i = 0; i < m_width-2; ++i) {
            new_neighbours = f(1);
            click();
        }

        new_neighbours = 0;
        click();
    };

    m_active.swap(m_future);
}

void World::resize(size_t width, size_t height) {
    if (width % cells_per_element != 0)
        throw std::runtime_error{"Width must be a multiple of cells_per_element."};
    m_width = width/cells_per_element;
    m_height = height;
    m_active.resize(m_width*(m_height+2));
    m_future.resize(m_width*(m_height+2));
}

size_t World::width() const {
    return cells_per_element*m_width;
}

size_t World::height() const {
    return m_height;
}

Cell World::get(size_t x, size_t y) const {
    auto ix = index(x, y);
    auto jx = bits_per_cell * (x % cells_per_element);
    return to_cell(1 & (m_active[ix] >> jx));
}

void World::set(size_t x, size_t y, Cell cell) {
    auto ix = index(x, y);
    auto jx = bits_per_cell * (x % cells_per_element);
    if (cell == alive)
        m_active[ix] |= element_type(1u) << jx;
    else
        m_active[ix] &= ~(element_type(1u) << jx);
}

size_t World::index(size_t x, size_t y) const {
    return (y+1) * m_width + (x / cells_per_element);
}

