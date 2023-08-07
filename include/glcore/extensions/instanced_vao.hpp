/**
 * @file instanced_vao.hpp
 * @author Dario Loi
 * @brief Instanced Vertex Array Object (VAO) wrapper.
 * @version 0.1
 * @date 2023-08-07
 * 
 * @copyright MIT License
 * 
 */

#pragma once

#include "gl_functions.hpp"
#include "vertex_buffer_layout.hpp"
#include "instanced_vbo.hpp"
#include "index_buffer.hpp"
#include "buffer_utility.hpp"

namespace glcore {

class instanced_vao {

private:
    uint32_t m_id{};

public:

    instanced_vbo m_vbo; // accessible to reduce boilerplate/overhead
    index_buffer m_ibo;

    instanced_vao() noexcept;
    ~instanced_vao();

    void bind() const;
    void unbind() const;

    void set_vertex_buffer(instanced_vbo&& vbo);
    void set_index_buffer(index_buffer&& ibo);

};

/*

    IMPLEMENTATIONS

*/

instanced_vao::instanced_vao() noexcept
{
    glGenVertexArrays(1, &m_id);
}

instanced_vao::~instanced_vao()
{
    glDeleteVertexArrays(1, &m_id);
}
void instanced_vao::bind() const
{
    glBindVertexArray(m_id);
}

void instanced_vao::unbind() const
{
    glBindVertexArray(0);
}

void instanced_vao::set_vertex_buffer(instanced_vbo&& vbo) {

    m_vbo = std::move(vbo);

    glBindVertexArray(m_id);
    m_vbo.bind();

    int attrib_index {};
    for (const auto& [type, name, offset] : m_vbo.model_layout().data()) {
        glEnableVertexAttribArray(attrib_index);
        glVertexAttribPointer(
            attrib_index++,
            shader_data_type::component_count(type),
            shader_data_type::to_opengl_type(type),
            GL_FALSE,
            m_vbo.model_layout().stride(),
            reinterpret_cast<const void*>(offset));
    }

    for (const auto& [type, name, offset] : m_vbo.instance_layout().data()) {
        glEnableVertexAttribArray(attrib_index);
        glVertexAttribPointer(
            attrib_index++,
            shader_data_type::component_count(type),
            shader_data_type::to_opengl_type(type),
            GL_FALSE,
            m_vbo.instance_layout().stride(),
            reinterpret_cast<const void*>(offset));
        glVertexAttribDivisor(attrib_index - 1, 1);
    }
}

void instanced_vao::set_index_buffer(index_buffer&& ibo) {
    glBindVertexArray(m_id);
    ibo.bind();

    m_ibo = std::move(ibo);

}

} // namespace glcore