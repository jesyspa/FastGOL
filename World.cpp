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
      m_active(m_width*m_height), m_future(m_active)
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
    element_type* in = m_active.data();
    element_type* out = m_future.data();
    element_type old_neighbours = 0;
    element_type neighbours = 0;
    element_type new_neighbours = 0;

    auto click = [&] {
        element_type in_value = *in;
        for (size_t shift = 1; shift < bits_per_cell; ++shift)
            in_value |= in_value << 1;
        element_type out_value = 0;
        element_type total_neighbours = (old_neighbours >> max_shift)
                + (neighbours << bits_per_cell)
                + neighbours
                + (neighbours >> bits_per_cell)
                + (new_neighbours << max_shift);

        element_type result = total_neighbours ^ in_value;
        for (size_t pos = 0; pos < cells_per_element; ++pos) {
            size_t self = result >> bits_per_cell*pos & value_mask;
            out_value |= live_data[self] << (bits_per_cell*pos);
        }
        *out = out_value;
        ++in;
        ++out;
        old_neighbours = neighbours;
        neighbours = new_neighbours;
    };

    auto do_row = [&](auto f) {
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

    do_row([&](int i) { return in[i] + in[m_width+i]; });
    for (size_t j = 0; j < m_height-2; ++j)
        do_row([&](int i) { return in[-m_width+i] + in[i] + in[m_width+i]; });
    do_row([&](int i) { return in[-m_width+i] + in[i]; });

    m_active.swap(m_future);
}

void World::resize(size_t width, size_t height) {
    if (width % cells_per_element != 0)
        throw std::runtime_error{"Width must be a multiple of cells_per_element."};
    m_width = width/cells_per_element;
    m_height = height;
    m_active.resize(m_width*m_height);
    m_future.resize(m_width*m_height);
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
    return y * m_width + (x / cells_per_element);
}

