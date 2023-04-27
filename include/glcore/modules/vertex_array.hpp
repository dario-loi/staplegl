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
class vertex_array {
public:
    vertex_array();
    ~vertex_array();

    void bind() const;
    void unbind() const;
    void add_vertex_buffer(const vertex_buffer& vbo);
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