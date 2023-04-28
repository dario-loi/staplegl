/**
 * @file vertex_array.hpp
 * @author Christian Panov
 * @brief Vertex Array Object (VAO) wrapper.
 * @version 0.1
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
    void add_vertex_buffer(const vertex_buffer& vbo);

    /**
     * @brief Set the index buffer object
     *
     * @param ibo the index buffer object to set.
     * @see index_buffer.hpp
     */
    void set_index_buffer(const index_buffer& ibo);

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

void vertex_array::add_vertex_buffer(const vertex_buffer& vbo)
{
    glBindVertexArray(m_id);
    vbo.bind();

    int attrib_index {};
    for (const auto& [type, name, offset] : vbo.layout().data()) {
        glEnableVertexAttribArray(attrib_index);
        glVertexAttribPointer(
            attrib_index++,
            shader_data_type::component_count(type),
            shader_data_type::to_opengl_type(type),
            GL_FALSE,
            vbo.layout().stride(),
            reinterpret_cast<const void*>(offset));
    }

    m_vertex_buffers.push_back(vbo);
}

void vertex_array::set_index_buffer(const index_buffer& ibo)
{
    glBindVertexArray(m_id);
    ibo.bind();

    m_index_buffer = ibo;
}
}