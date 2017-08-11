#include "World.hpp"
#include <algorithm>

namespace {
    size_t const row_size = 16;
    Cell live_data[row_size * 2];
    void init_live_data_impl() {
        std::fill(std::begin(live_data), std::end(live_data), dead);
        live_data[3] = alive;
        live_data[row_size + 3] = alive;
        live_data[row_size + 4] = alive;
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
    : m_width(width), m_height(height),
      m_active(width*height), m_future(m_active)
{
    init_live_data();
}

World::World(IWorld const& world) : World() {
    init_live_data();
    copy(world);
}

void World::update() {
    // cell itself always counts as a neighbour
    int neighbours = 0;
    Cell* in = m_active.data();
    Cell* out = m_future.data();
    auto click = [&in, &out, &neighbours]() {
        *out = live_data[row_size * (int)*in + neighbours];
    };
    auto next = [&in, &out]() {
        ++in;
        ++out;
    };

    // cell 0, 0
    neighbours = here(in) + down(in) + right(in) + down_right(in);
    click();
    for (size_t i = 0; i < m_width - 2; ++i) {
        // cell i+1, 0
        next();
        neighbours += right(in) + down_right(in);
        click();
        neighbours -= left(in) + down_left(in);
    }
    // cell width-1, 0
    next();
    click();

    // looparound
    next();

    for (size_t j = 0; j < m_height - 2; ++j) {
        // cell 0, j+1
        neighbours = here(in) + up(in) + up_right(in) + right(in) + down(in) + down_right(in);
        click();

        for (size_t i = 0; i < m_width - 2; ++i) {
            // cell i+1, j+1
            next();
            neighbours += up_right(in) + right(in) + down_right(in);
            click();
            neighbours -= up_left(in) + left(in) + down_left(in);
        }

        // cell width-1, j+1
        next();
        click();

        // looparound
        next();
        neighbours = 0;
    }

    // cell 0, height-1
    neighbours = here(in) + up(in) + right(in) + up_right(in);
    click();
    for (size_t i = 0; i < m_width - 2; ++i) {
        // cell i+1, height-1
        next();
        neighbours += right(in) + up_right(in);
        click();
        neighbours -= left(in) + up_left(in);
    }
    // cell width-1, height-1
    next();
    click();

    m_active.swap(m_future);
}

void World::resize(size_t width, size_t height) {
    m_width = width;
    m_height = height;
    m_active.resize(width*height);
    m_future.resize(width*height);
}

size_t World::width() const {
    return m_width;
}

size_t World::height() const {
    return m_height;
}

Cell World::get(size_t x, size_t y) const {
    return get_active(x, y);
}

void World::set(size_t x, size_t y, Cell cell) {
    m_active[index(x, y)] = cell;
}

Cell World::up_left(Cell* in) const { return in[-m_width-1]; }
Cell World::up(Cell* in) const { return in[-m_width]; }
Cell World::up_right(Cell* in) const { return in[-m_width+1]; }
Cell World::down_left(Cell* in) const { return in[m_width-1]; }
Cell World::down(Cell* in) const { return in[m_width]; }
Cell World::down_right(Cell* in) const { return in[m_width+1]; }
Cell World::left(Cell* in) const { return in[-1]; }
Cell World::right(Cell* in) const { return in[1]; }
Cell World::here(Cell* in) const { return in[0]; }

size_t World::index(size_t x, size_t y) const {
    return y * m_width + x;
}

Cell World::get_active(size_t x, size_t y) const {
    return m_active[index(x, y)];
}

void World::set_future(size_t x, size_t y, Cell cell) {
    m_future[index(x, y)] = cell;
}
