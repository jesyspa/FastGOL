#include "World.hpp"
#include <algorithm>
#include <stdexcept>

namespace {
    // Make an element where all cells are alive, and no other bits are set.
    constexpr World::element_type make_all_one_mask() {
        World::element_type mask = 1;
        for (size_t i = 0; i < World::cells_per_element; ++i)
            mask |= mask << World::bits_per_cell;
        return mask;
    }

    constexpr World::element_type all_one_mask = make_all_one_mask();
}

World::World() : m_width(), m_height(), m_active(), m_future()
{ }

World::World(size_t width, size_t height)
    : m_width(width/cells_per_element), m_height(height),
      m_active(m_width*(m_height+2)), m_future(m_active)
{
    test_size(width, height);
}

World::World(BaseWorld const& world) : World() {
    copy(world);
}

void World::update() {
    // Cells are stored as 0001s (alive) and 0000s (dead).  By adding these values, we can quickly
    // get a count of the number of neighbours a cell has.  If the cell is alive, we take the
    // bitwise complement of the resulting value; we then check whether the resulting bit pattern
    // leaves the cell alive (this is unambiguous) and store that.
    element_type* in = m_active.data() + m_width;
    element_type* out = m_future.data() + m_width;
    element_type old_neighbours = 0;
    element_type neighbours = 0;
    element_type new_neighbours = 0;

    auto process_one = [&] {
        element_type in_value = *in << 3;
        // We need the old and new neighbour counts in order to get them right for the leftmost
        // and rightmost cell.
        element_type total_neighbours = (old_neighbours >> max_shift)
                + (neighbours << bits_per_cell)
                + neighbours
                + (neighbours >> bits_per_cell)
                + (new_neighbours << max_shift);

        element_type result = total_neighbours ^ in_value;
        element_type result_1 = result >> 1;
        element_type result_2 = result >> 2;
        element_type result_3 = result >> 3;
        element_type sub = ~result_2 & result_1 & result;
        element_type option_a = ~result_3 & sub;
        element_type option_b = result_3 & sub;
        element_type option_c = result_3 & result_2 & ~result_1 & ~result;
        *out = (option_a | option_b | option_c) & all_one_mask;

        ++in;
        ++out;
        old_neighbours = neighbours;
        neighbours = new_neighbours;
    };


    for (size_t j = 0; j < m_height; ++j) {
        auto f = [&](int i) { return in[-m_width+i] + in[i] + in[m_width+i]; };
        old_neighbours = 0;
        neighbours = f(0);
        size_t i = 0;
        goto middle;
        while (i < m_width/unroll_factor) {
            new_neighbours = f(1);
            process_one();
        middle:
            new_neighbours = f(1);
            process_one();
            new_neighbours = f(1);
            process_one();
            new_neighbours = f(1);
            process_one();
            new_neighbours = f(1);
            process_one();
            new_neighbours = f(1);
            process_one();
            new_neighbours = f(1);
            process_one();
            new_neighbours = f(1);
            process_one();
            ++i;
        }

        new_neighbours = 0;
        process_one();
    };

    m_active.swap(m_future);
}

void World::resize(size_t width, size_t height) {
    test_size(width, height);
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
    return to_cell(element_type(1) & (m_active[ix] >> jx));
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

void World::test_size(size_t width, size_t height) const {
    (void)height; // all heights are fine at the moment
    // This is somewhat redundant, as cells_per_element is larger than 2.
    if (width < 2)
        throw std::runtime_error{"Width must be at least two."};
    if (width % (cells_per_element * 8) != 0)
        throw std::runtime_error{"Width must be a multiple of cells_per_element times unroll_factor."};
}
