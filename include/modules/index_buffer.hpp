/**
 * @file index_buffer.hpp
 * @authors Christian Panov, Dario Loi
 * @brief Element Buffer Object (EBO) wrapper.
 *
 * @date 2023-04-28
 *
 * @copyright MIT License
 * 
 * @details Wraps EBOs allowing for easy creation and usage. Element buffer objects 
 * are GPU buffers that store indices that OpenGL uses to figure out the order 
 * in which to render primitives that are stored in a VBO. <br>
 * 
 * This class allows simple creation and management of EBOs with RAII semantics.
 *
 * @see vertex_buffer
 * @see https://www.khronos.org/opengl/wiki/Vertex_Specification#Element_Buffer_Object
 */

#pragma once

#include "gl_functions.hpp"
#include <cstdint>
#include <iostream>
#include <span>

namespace staplegl {

/**
 * @brief Element Buffer Object (EBO) wrapper.
 *
 * @details Element Buffer Objects are OpenGL objects that store indices that OpenGL uses to
 * render primitives, they can be bound to a VAO to specify the order in which to render
 * the vertices contained in its VBOs. <br>
 * 
 * EBOs are used to reduce the amount of data that needs to be sent to the GPU, by allowing
 * the reuse of vertices.
 *
 * @see vertex_array
 * @see vertex_buffer
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

    /**
     * @brief Destroy the index buffer object
     * 
     */
    ~index_buffer();

    index_buffer(const index_buffer&) = delete;
    auto operator=(const index_buffer&) -> index_buffer& = delete;

    /**
     * @brief Construct a new index buffer object
     * 
     * @param other the other index buffer object to move from.
     *
     * @details Moving from an index buffer object is cheap, as it only involves copying the
     * internal OpenGL ID, the other object is therefore left with an ID of 0.
     */
    index_buffer(index_buffer&&) noexcept;

    /**
     * @brief Move assignment operator.
     * 
     * @param other the other index buffer object to move from.
     * 
     * @return index_buffer& The moved object.
     */
    auto operator=(index_buffer&&) noexcept -> index_buffer&;

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
     * @return std::int32_t, the number of indices.
     */
    [[nodiscard]] constexpr auto count() const -> std::int32_t;

    /**
     * @brief Get the ID of the index buffer object.
     * 
     * @return std::uint32_t the ID of the index buffer object.
     */
    [[nodiscard]] constexpr auto id() const -> std::uint32_t;

private:
    std::uint32_t m_id {};
    std::int32_t m_count {};
};

inline index_buffer::index_buffer(std::span<const std::uint32_t> indices) noexcept
    : m_count { static_cast<int32_t>(indices.size()) }
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
    static_cast<ptrdiff_t>(indices.size_bytes()), 
    indices.data(), 
    GL_STATIC_DRAW);
}

inline index_buffer::~index_buffer()
{
    if (m_id != 0) {
        glDeleteBuffers(1, &m_id);
    }
}

inline index_buffer::index_buffer(index_buffer&& other) noexcept
    : m_id { other.m_id }
    , m_count { other.m_count }
{
    other.m_id = 0;
}

inline auto index_buffer::operator=(index_buffer&& other) noexcept -> index_buffer&
{
    if (this != &other) {
        m_id = other.m_id;
        m_count = other.m_count;

        other.m_id = 0;
    }

    return *this;
}

inline void index_buffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

inline void index_buffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

constexpr auto index_buffer::count() const -> std::int32_t
{
    return m_count;
}
}