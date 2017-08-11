#include "World.hpp"
#include <algorithm>
#include <stdexcept>

namespace {
    size_t const row_size = 16;
    World::element_type live_data[row_size * 2];
    void init_live_data_impl() {
        live_data[3] = 1;
        live_data[row_size + 3] = 1;
        live_data[row_size + 4] = 1;
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
    element_type neighbours = in[0] + in[m_width];
    element_type new_neighbours = in[1] + in[m_width+1];

    element_type value = 0;
    auto click_zero = [&] {
        size_t left_n = old_neighbours >> bits_per_cell*(cells_per_element-1);
        size_t mid_n = neighbours & value_mask;
        size_t right_n = (neighbours >> bits_per_cell) & value_mask;
        size_t num_neighbours = left_n + mid_n + right_n;
        size_t self = *in % 2;
        value = live_data[row_size * self + num_neighbours];
    };
    auto click_pos = [&](int pos) {
        size_t left_n = (neighbours >> bits_per_cell*(pos-1)) & value_mask;
        size_t mid_n = (neighbours >> bits_per_cell*pos) & value_mask;
        size_t right_n = (neighbours >> bits_per_cell*(pos+1)) & value_mask;
        size_t num_neighbours = left_n + mid_n + right_n;
        size_t self = (*in >> bits_per_cell*pos) % 2;
        value |= live_data[row_size * self + num_neighbours] << (bits_per_cell*pos);
    };
    auto click_max = [&] {
        size_t left_n = (neighbours >> bits_per_cell*(cells_per_element-2)) & value_mask;
        size_t mid_n = (neighbours >> bits_per_cell*(cells_per_element-1));
        size_t right_n = new_neighbours & value_mask;
        size_t num_neighbours = left_n + mid_n + right_n;
        size_t self = (*in >> bits_per_cell*(cells_per_element-1)) % 2;
        value |= live_data[row_size * self + num_neighbours] << bits_per_cell*(cells_per_element-1);
    };
    auto click = [&] {
        click_zero();
        for (size_t pos = 1; pos < cells_per_element-1; ++pos)
            click_pos(pos);
        click_max();
        *out = value;
        ++in;
        ++out;
        old_neighbours = neighbours;
        neighbours = new_neighbours;
    };

    // Top row
    // First cell
    click();

    // Other cells
    for (size_t i = 0; i < m_width-2; ++i) {
        new_neighbours = in[1] + in[m_width+1];
        click();
    }
    // Last cell
    new_neighbours = 0;
    click();

    // Other rows
    for (size_t j = 0; j < m_height-2; ++j) {
        old_neighbours = 0;
        neighbours = in[-m_width] + in[0] + in[m_width];
        new_neighbours = in[-m_width+1] + in[1] + in[m_width+1];

        // First cell
        click();

        // Other cells
        for (size_t i = 0; i < m_width-2; ++i) {
            new_neighbours = in[-m_width+1] + in[1] + in[m_width+1];
            click();
        }
        // Last cell
        new_neighbours = 0;
        click();
    }

    // Last row
    old_neighbours = 0;
    neighbours = in[-m_width] + in[0];
    new_neighbours = in[-m_width+1] + in[1];

    // First cell
    click();

    // Other cells
    for (size_t i = 0; i < m_width-2; ++i) {
        new_neighbours = in[-m_width+1] + in[1];
        click();
    }
    // Last cell
    new_neighbours = 0;
    click();

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

