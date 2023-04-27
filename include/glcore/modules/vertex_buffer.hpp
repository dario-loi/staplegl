/**
 * @file vertex_buffer.hpp
 * @author Christian Panov
 * @brief Vertex Buffer Object (VBO) wrapper.
 * @version 0.1
 * @date 2023-04-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include "gl_functions.hpp"
#include "vertex_buffer_layout.hpp"

namespace glcore {
class vertex_buffer {
public:
    vertex_buffer() = default;
    vertex_buffer(float* vertices, std::uint32_t size);
    vertex_buffer(float* vertices, std::uint32_t size, const vertex_buffer_layout& layout);
    ~vertex_buffer();

    void bind() const;
    void unbind() const;
    void set_layout(const vertex_buffer_layout& layout);
    const vertex_buffer_layout& layout() const;

private:
    std::uint32_t m_id {};
    vertex_buffer_layout m_layout;
};

/*

        IMPLEMENTATIONS

*/

vertex_buffer::vertex_buffer(float* vertices, std::uint32_t size)
{
    glCreateBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

vertex_buffer::vertex_buffer(float* vertices, std::uint32_t size, const vertex_buffer_layout& layout)
    : vertex_buffer { vertices, size }
{
    m_layout = layout;
}

vertex_buffer::~vertex_buffer()
{
    glDeleteBuffers(1, &m_id);
}

void vertex_buffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void vertex_buffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vertex_buffer::set_layout(const vertex_buffer_layout& layout)
{
    m_layout = layout;
}

const vertex_buffer_layout& vertex_buffer::layout() const
{
    return m_layout;
}
}