/**
 * @file vertex_array.hpp
 * @author Christian Panov
 * @brief Vertex Array Object (VAO) wrapper.
 *
 * @date 2023-04-28
 *
 * @copyright MIT License
 *
 */

#pragma once
#include "gl_functions.hpp"
#include "index_buffer.hpp"
#include "vertex_buffer.hpp"
#include "vertex_buffer_inst.hpp"

#include <cassert>
#include <cstdint>
#include <list>
#include <utility>
#include <variant>

namespace glcore {

/**
 * @brief Vertex Array Object (VAO) wrapper.
 *
 * @details Vertex Array Objects are OpenGL objects that store all of the state needed to supply vertex data,
 * that is, an index buffer (also called an Element Buffer Object) and one or more vertex buffers.
 *
 * Rendering using VAOs is advised as it simplifies the process of rendering multiple objects with different
 * vertex data and different rendering modes.
 *
 * @see https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Array_Object
 * @see vertex_buffer.hpp
 * @see index_buffer.hpp
 * @see vertex_buffer_layout.hpp
 */
class vertex_array {
public:
    vertex_array();
    ~vertex_array();

    using vbo_variant = std::variant<glcore::vertex_buffer, glcore::vertex_buffer_inst>;
    using iterator_t = std::list<vbo_variant>::iterator;

    /**
     * @brief Bind the vertex array object.
     *
     */
    void bind() const;

    /**
     * @brief Unbind the vertex array object.
     *
     */
    void unbind() const;

    /**
     * @brief Add a vertex buffer to the vertex array object.
     *
     *
     * @param vbo the vertex buffer object to add.
     * @see vertex_buffer.hpp
     */
    iterator_t add_vertex_buffer(vertex_buffer&& vbo);

    /**
     * @brief Add an instanced vertex buffer to the vertex array object.
     *
     * @param vbo the instanced vertex buffer object to add.
     * @see vertex_buffer_inst.hpp
     *
     *
     */
    iterator_t add_vertex_buffer(vertex_buffer_inst&& vbo);

    /**
     * @brief Set the index buffer object
     *
     * @param ibo the index buffer object to set.
     * @see index_buffer.hpp
     */
    void set_index_buffer(index_buffer&& ibo);

private:
    std::uint32_t m_id {};
    std::list<vbo_variant> m_vertex_buffers;
    index_buffer m_index_buffer;
    uint32_t attrib_index {};
};

/*

        IMPLEMENTATIONS

*/

vertex_array::vertex_array()
{
    glCreateVertexArrays(1, &m_id);
}

vertex_array::~vertex_array()
{
    glDeleteVertexArrays(1, &m_id);
}

void vertex_array::bind() const
{
    glBindVertexArray(m_id);
}

void vertex_array::unbind() const
{
    glBindVertexArray(0);
}

vertex_array::iterator_t vertex_array::add_vertex_buffer(vertex_buffer&& vbo)
{
    glBindVertexArray(m_id);
    m_vertex_buffers.push_back(std::move(vbo));

    // get a reference to the newly added vertex buffer from the variant in the vector
    vertex_buffer const& vbo_ref = std::get<vertex_buffer>(m_vertex_buffers.back());

    vbo_ref.bind();

    for (const auto& [type, name, offset] : vbo_ref.layout().data()) {
        glEnableVertexAttribArray(attrib_index);
        glVertexAttribPointer(
            attrib_index++,
            shader_data_type::component_count(type),
            shader_data_type::to_opengl_type(type),
            GL_FALSE,
            vbo_ref.layout().stride(),
            reinterpret_cast<const void*>(offset));
    }

    return std::prev(m_vertex_buffers.end());
}

vertex_array::iterator_t vertex_array::add_vertex_buffer(vertex_buffer_inst&& vbo)
{
    glBindVertexArray(m_id);
    m_vertex_buffers.push_back(std::move(vbo));

    vertex_buffer_inst const& vbo_ref = std::get<vertex_buffer_inst>(m_vertex_buffers.back());

    vbo_ref.bind();

    for (const auto& [type, name, offset] : vbo_ref.layout().data()) {
        glEnableVertexAttribArray(attrib_index);
        glVertexAttribPointer(
            attrib_index++,
            shader_data_type::component_count(type),
            shader_data_type::to_opengl_type(type),
            GL_FALSE,
            vbo_ref.layout().stride(),
            reinterpret_cast<const void*>(offset));
        glVertexAttribDivisor(attrib_index - 1, 1);
    }

    return std::prev(m_vertex_buffers.end());
}

void vertex_array::set_index_buffer(index_buffer&& ibo)
{
    m_index_buffer = std::move(ibo);

    glBindVertexArray(m_id);
    m_index_buffer.bind();
}
}