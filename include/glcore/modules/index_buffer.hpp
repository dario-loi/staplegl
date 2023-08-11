/**
 * @file index_buffer.hpp
 * @author sChristian Panov, Dario Loi
 * @brief Element Buffer Object (EBO) wrapper.
 *
 * @date 2023-04-28
 *
 * @copyright MIT License
 *
 */

#pragma once

#include "gl_functions.hpp"
#include <cstdint>
#include <iostream>
#include <span>

namespace glcore {

/**
 * @brief Element Buffer Object (EBO) wrapper.
 *
 * @details Element Buffer Objects are OpenGL objects that store indices that OpenGL uses to
 * render primitives, they are used in conjunction with Vertex Array Objects. <br>
 * EBOs are used to reduce the amount of data that needs to be sent to the GPU, by allowing
 * the reuse of vertices.
 *
 * @see https://www.khronos.org/opengl/wiki/Vertex_Specification#Element_Buffer_Object
 *
 */
class index_buffer {
public:
    index_buffer() = default;

    /**
     * @brief Construct a new index buffer object
     *
     * @param indices a pointer to the indices array, can be any contiguous container of std::uint32_t.
     * @param count the number of indices in the array.
     */
    index_buffer(std::span<const std::uint32_t> indices) noexcept;
    ~index_buffer();

    index_buffer(const index_buffer&) = delete;
    index_buffer& operator=(const index_buffer&) = delete;

    index_buffer(index_buffer&&) noexcept;
    index_buffer& operator=(index_buffer&&) noexcept;

    /**
     * @brief Bind the index buffer object.
     *
     */
    void bind() const;

    /**
     * @brief Unbind the index buffer object.
     *
     */
    void unbind() const;

    /**
     * @brief Get the number of indices in the index buffer object.
     *
     * @return std::uint32_t, the number of indices.
     */
    std::size_t count() const;

private:
    std::uint32_t m_id {};
    std::size_t m_count {};
};

index_buffer::index_buffer(std::span<const std::uint32_t> indices) noexcept
    : m_count { indices.size() }
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);
}

index_buffer::~index_buffer()
{
    if (m_id != 0) {
        glDeleteBuffers(1, &m_id);
    }
}

index_buffer::index_buffer(index_buffer&& other) noexcept
    : m_id { other.m_id }
    , m_count { other.m_count }
{
    other.m_id = 0;
}

index_buffer& index_buffer::operator=(index_buffer&& other) noexcept
{
    if (this != &other) {
        m_id = other.m_id;
        m_count = other.m_count;

        other.m_id = 0;
    }

    return *this;
}

void index_buffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void index_buffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::size_t index_buffer::count() const
{
    return m_count;
}
}