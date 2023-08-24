#pragma once

#include "gl_functions.hpp"
#include "vertex_buffer_layout.hpp"

#include <span>

namespace glcore {

class uniform_buffer {

    uniform_buffer(std::span<const float> contents, vertex_buffer_layout layout);
    uniform_buffer(vertex_buffer_layout layout);

    uniform_buffer(const uniform_buffer&) = delete;
    uniform_buffer& operator=(const uniform_buffer&) = delete;

    uniform_buffer(uniform_buffer&&) noexcept;
    [[nodiscard]] uniform_buffer& operator=(uniform_buffer&&) noexcept;

    void bind() const;
    void unbind() const;

    ~uniform_buffer();

private:
    std::uint32_t m_id {};
    vertex_buffer_layout m_layout;

}; // class uniform_buffer

uniform_buffer::uniform_buffer(std::span<const float> contents, vertex_buffer_layout layout)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
    glBufferData(GL_UNIFORM_BUFFER, contents.size_bytes(), contents.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

uniform_buffer::uniform_buffer(vertex_buffer_layout layout)
    : m_layout { layout }
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
    glBufferData(GL_UNIFORM_BUFFER, layout.stride(), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void uniform_buffer::bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
}

void uniform_buffer::unbind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

uniform_buffer::~uniform_buffer()
{
    if (m_id != 0) {
        glDeleteBuffers(1, &m_id);
    }
}

uniform_buffer::uniform_buffer(uniform_buffer&& other) noexcept
    : m_id { other.m_id }
    , m_layout { other.m_layout }
{
    other.m_id = 0;
}

[[nodiscard]] uniform_buffer& uniform_buffer::operator=(uniform_buffer&& other) noexcept
{
    if (this != &other) {
        glDeleteBuffers(1, &m_id);
        m_id = other.m_id;
        m_layout = other.m_layout;

        other.m_id = 0;
    }

    return *this;
}

} // namespace glcore