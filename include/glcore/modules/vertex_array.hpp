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
    void add_vertex_buffer(vertex_buffer&& vbo);

    /**
     * @brief Set the index buffer object
     *
     * @param ibo the index buffer object to set.
     * @see index_buffer.hpp
     */
    void set_index_buffer(index_buffer&& ibo);

private:
    std::uint32_t m_id {};
    std::vector<vertex_buffer> m_vertex_buffers;
    index_buffer m_index_buffer;
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

void vertex_array::add_vertex_buffer(vertex_buffer&& vbo)
{
    m_vertex_buffers.push_back(std::move(vbo));

    vertex_buffer& vbo_ref = m_vertex_buffers.back();

    glBindVertexArray(m_id);
    vbo_ref.bind();

    int attrib_index {};
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

}

void vertex_array::set_index_buffer(index_buffer&& ibo)
{
    m_index_buffer = std::move(ibo);

    glBindVertexArray(m_id);
    m_index_buffer.bind();
}
}