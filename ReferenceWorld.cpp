#include "ReferenceWorld.hpp"

ReferenceWorld::ReferenceWorld(size_t width, size_t height)
    : m_width(width), m_height(height), m_data(width*height)
{}

void ReferenceWorld::update() {
    std::vector<Cell> new_data(m_width * m_height, dead);

    for (size_t y = 0; y < m_height; ++y) {
        for (size_t x = 0; x < m_width; ++x) {
            // note: cell itself counts as its own neighbour.
            unsigned neighbours = 0;
            for (int i = -1; i <= 1; ++i)
                for (int j = -1; j <= 1; ++j)
                    neighbours += get(x+i, y+j);

            if (get(x, y) == alive && 3 <= neighbours && neighbours <= 4)
                new_data[index(x, y)] = alive;
            if (get(x, y) == dead && neighbours == 3)
                new_data[index(x, y)] = alive;
        }
    }

    m_data = new_data;
}

void ReferenceWorld::resize(size_t width, size_t height) {
    m_width = width;
    m_height = height;
    m_data.resize(width*height);
}

size_t ReferenceWorld::width() const {
    return m_width;
}

size_t ReferenceWorld::height() const {
    return m_height;
}

Cell ReferenceWorld::get(size_t x, size_t y) const {
    if (valid(x, y))
        return m_data[index(x, y)];
    return dead;
}

void ReferenceWorld::set(size_t x, size_t y, Cell cell) {
    m_data[index(x, y)] = cell;
}

bool ReferenceWorld::valid(size_t x, size_t y) const {
    return x < m_width && y < m_height;
}

size_t ReferenceWorld::index(size_t x, size_t y) const {
    return y * m_width + x;
}

