/**
 * @file vertex_buffer.hpp
 * @author Christian Panov
 * @brief Vertex Buffer Object (VBO) wrapper.
 *
 * @date 2023-04-28
 *
 * @copyright MIT License
 *
 */

#pragma once
#include "gl_functions.hpp"
#include "vertex_buffer_layout.hpp"
#include <span>

namespace glcore {

enum driver_draw_hint {
    STATIC_DRAW = GL_STATIC_DRAW,
    DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
    STREAM_DRAW = GL_STREAM_DRAW
};

/**
 * @brief Vertex Buffer Object (VBO) wrapper.
 *
 * @details Vertex Buffer Objects are OpenGL objects that store an array of data in the GPU's memory,
 * the data is passed as a pointer and is then accessed through a user-specified layout.
 *
 * @see https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
 * @see vertex_buffer_layout.hpp
 */
class vertex_buffer {
public:
    /**
     * @brief Construct a new vertex buffer object
     *
     * @note The vertices array is copied into the GPU's memory, so it can be safely deleted after the call.
     * @note By passing an empty std::span, the VBO will be initialized with no data.
     *
     * @param vertices a pointer to the vertices array, can be any contiguous container of floats.
     * @param size the size of the vertices array in bytes.
     */
    vertex_buffer(std::span<const float> vertices) noexcept;
    vertex_buffer(std::span<const float> vertices, driver_draw_hint hint) noexcept;
    vertex_buffer(std::span<const float> vertices, const vertex_buffer_layout& layout);
    vertex_buffer(std::span<const float> vertices, const vertex_buffer_layout& layout,
        driver_draw_hint hint);
    ~vertex_buffer();

    // delete copy and assignment, only move is allowed
    vertex_buffer(const vertex_buffer&) = delete;
    vertex_buffer& operator=(const vertex_buffer&) = delete;

    vertex_buffer(vertex_buffer&&) noexcept;
    vertex_buffer& operator=(vertex_buffer&&) noexcept;

    /**
     * @brief Bind the vertex buffer object.
     *
     */
    void bind() const;

    /**
     * @brief Unbind the vertex buffer object.
     *
     */
    void unbind() const;

    /**
     * @brief Set the layout object
     *
     * @param layout the layout to be set.
     * @see vertex_buffer_layout.hpp
     */
    void set_layout(const vertex_buffer_layout& layout);
    [[nodiscard]] const vertex_buffer_layout& layout() const;

    /**
     * @brief Give new data to the vertex buffer object, overwriting the old one.
     *
     */

    void set_data(std::span<const float> vertices) noexcept;

    // UTILITIES

    /**
     * @brief Get the id of the vertex buffer object.
     *
     */
    constexpr std::uint32_t id() const noexcept { return m_id; }

    /**
     * @brief Get the size of the vertex buffer object in bytes.
     *
     */
    constexpr std::size_t size() const noexcept { return m_layout.stride(); }

protected:
    std::uint32_t m_id {};
    vertex_buffer_layout m_layout;
};

/*

        IMPLEMENTATIONS

*/

vertex_buffer::vertex_buffer(std::span<const float> vertices, driver_draw_hint hint) noexcept
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), hint);
}

vertex_buffer::vertex_buffer(std::span<const float> vertices) noexcept
    : vertex_buffer { vertices, driver_draw_hint::DYNAMIC_DRAW } // give the user the largest flexibility
{
}

vertex_buffer::vertex_buffer(std::span<const float> vertices, const vertex_buffer_layout& layout,
    driver_draw_hint hint)
    : vertex_buffer { vertices, hint }
{
    m_layout = layout;
}

vertex_buffer::vertex_buffer(std::span<const float> vertices, const vertex_buffer_layout& layout)
    : vertex_buffer { vertices, layout, driver_draw_hint::DYNAMIC_DRAW } // give the user the largest flexibility
{
}

vertex_buffer::~vertex_buffer()
{
    if (m_id != 0) {
        glDeleteBuffers(1, &m_id);
    }
}

vertex_buffer::vertex_buffer(vertex_buffer&& other) noexcept
    : m_id { other.m_id }
    , m_layout { other.m_layout }
{
    other.m_id = 0;
}

vertex_buffer& vertex_buffer::operator=(vertex_buffer&& other) noexcept
{
    if (this != &other) {
        glDeleteBuffers(1, &m_id);
        m_id = other.m_id;
        m_layout = other.m_layout;

        other.m_id = 0;
    }

    return *this;
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

[[nodiscard]] const vertex_buffer_layout& vertex_buffer::layout() const
{
    return m_layout;
}

void vertex_buffer::set_data(std::span<const float> vertices) noexcept
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);
}

} // namespace glcore