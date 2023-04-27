/**
 * @file index_buffer.hpp
 * @author Christian Panov
 * @brief Element Buffer Object (EBO) wrapper.
 * @version 0.1
 * @date 2023-04-28
 *
 * @copyright MIT License
 *
 */

#pragma once

#include "gl_functions.hpp"
#include <cstdint>

namespace glcore {
class index_buffer {
public:
    index_buffer() = default;
    index_buffer(std::uint32_t* indices, std::uint32_t count);
    ~index_buffer();

    void bind() const;
    void unbind() const;
    std::uint32_t count() const;

private:
    std::uint32_t m_id {};
    std::uint32_t m_count {};
};

index_buffer::index_buffer(std::uint32_t* indices, std::uint32_t count)
    : m_count { count }
{
    glCreateBuffers(1, &m_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * count, indices, GL_STATIC_DRAW);
}

index_buffer::~index_buffer()
{
    glDeleteBuffers(1, &m_id);
}

void index_buffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void index_buffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::uint32_t index_buffer::count() const
{
    return m_count;
}
}