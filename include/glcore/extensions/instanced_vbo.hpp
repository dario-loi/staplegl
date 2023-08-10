/**
 * @file instanced_vbo.hpp
 * @author Dario Loi
 * @brief Vertex Buffer Object (VBO) wrapper for batch rendering.
 *
 * @date 2023-08-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include "gl_functions.hpp"
#include "vertex_buffer_layout.hpp"
#include "buffer_utility.hpp"
#include <cstring> //memmove
#include <iostream>

namespace glcore {

/**
 * @brief VBO used for batch rendering.
 * 
 * @details Abstracts away a VBO that stores both model data and instance data, allows one to 
 * append and delete instances from the VBO.
 * 
 * @note This is not meant to be used directly, but rather in conjunction with an instanced VAO.
 * 
 */
class instanced_vbo {

private:

    using layout_t = vertex_buffer_layout;

    uint32_t m_id{};
    size_t capacity{};
    uint32_t m_model_size{};
    size_t inst_count{};

    layout_t m_model_layout{};
    layout_t m_instance_layout{};

public:

    instanced_vbo() = default;

    instanced_vbo(const float* model_vertices, uint32_t model_size, const layout_t& inst_layout,
    const layout_t& model_layout) noexcept;
    ~instanced_vbo();

    instanced_vbo(const instanced_vbo&) = delete;
    instanced_vbo& operator=(const instanced_vbo&) = delete;

    instanced_vbo(instanced_vbo&&) noexcept;
    instanced_vbo& operator=(instanced_vbo&&) noexcept;

    void bind() const;
    void unbind() const;

    void add_instance(const float* instance_data);
    size_t delete_instance(uint32_t index);
    void update_instance(uint32_t index, const float* instance_data);
    void clear_instances();

    size_t instance_count() const { return inst_count; }

    void set_model_layout(const layout_t& layout) { m_model_layout = layout; }
    void set_instance_layout(const layout_t& layout) { m_instance_layout = layout; }

    const layout_t& model_layout() const { return m_model_layout; }
    const layout_t& instance_layout() const { return m_instance_layout; }

    uint32_t get_model_size() const { return m_model_size; }
    uint32_t get_instance_size() const { return m_instance_layout.stride(); }
    size_t get_capacity() const { return capacity; }

};

/*

    IMPLEMENTATIONS

*/

instanced_vbo::instanced_vbo(const float* model_vertices, uint32_t model_size, const layout_t& inst_layout,
    const layout_t& model_layout) noexcept
    : capacity { model_size + 4096 }
    , m_model_size { model_size }
    , inst_count { 0 }
    , m_model_layout { model_layout }
    , m_instance_layout { inst_layout }
{
    glCreateBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, capacity, nullptr, GL_DYNAMIC_DRAW);

    // copy model data
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_size, model_vertices);

    }

    instanced_vbo::~instanced_vbo()
    {
        if (m_id != 0) {
            glDeleteBuffers(1, &m_id);
        }
    }

    instanced_vbo::instanced_vbo(instanced_vbo&& other) noexcept
        : m_id{ other.m_id }, capacity{ other.capacity }, m_model_size{ other.m_model_size}, inst_count{ other.inst_count },
        m_model_layout{ other.m_model_layout }, m_instance_layout{ other.m_instance_layout }
    {
        other.m_id = 0;
    }

    instanced_vbo& instanced_vbo::operator=(instanced_vbo&& other) noexcept
    {
        if (this != &other) {
            glDeleteBuffers(1, &m_id);
            m_id = other.m_id;
            capacity = other.capacity;
            m_model_size = other.m_model_size;
            inst_count = other.inst_count;
            m_model_layout = other.m_model_layout;
            m_instance_layout = other.m_instance_layout;

            other.m_id = 0;
        }
        return *this;
    }

    void instanced_vbo::bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
    }

    void instanced_vbo::unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void instanced_vbo::add_instance(const float* instance_data)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);

        uint32_t inst_size = get_instance_size();

        if ((get_model_size() + (inst_count + 1) * get_instance_size()) > capacity) {
            capacity += inst_size + 4096;

            std::uint32_t new_id{};

            glGenBuffers(1, &new_id);
            glBindBuffer(GL_ARRAY_BUFFER, new_id);
            glBufferData(GL_ARRAY_BUFFER, capacity, nullptr, GL_DYNAMIC_DRAW);

            glcore::move_buffer_data(m_id, new_id);

            glDeleteBuffers(1, &m_id);
            m_id = new_id;
            glBindBuffer(GL_ARRAY_BUFFER, m_id);

            int attrib_index {};
            for (const auto& [type, name, offset] : model_layout().data()) {
                glEnableVertexAttribArray(attrib_index);
                glVertexAttribPointer(
                    attrib_index++,
                    shader_data_type::component_count(type),
                    shader_data_type::to_opengl_type(type),
                    GL_FALSE,
                    model_layout().stride(),
                    reinterpret_cast<const void*>(offset));
            }

            // all instance attributes come after the model attributes
            for (const auto& [type, name, offset] : instance_layout().data()) {
                glEnableVertexAttribArray(attrib_index);
                glVertexAttribPointer(
                    attrib_index++,
                    shader_data_type::component_count(type),
                    shader_data_type::to_opengl_type(type),
                    GL_FALSE,
                    instance_layout().stride(),
                    reinterpret_cast<const void*>(get_model_size() + offset));
                glVertexAttribDivisor(attrib_index - 1, 1);
            }
        }

        glBufferSubData(GL_ARRAY_BUFFER, get_model_size() + (inst_count++ * get_instance_size()), inst_size, instance_data);
    }

    size_t instanced_vbo::delete_instance(uint32_t index)
    {
        if (index >= inst_count) {
            return 0; // TODO: std::optional
        }

        // swap instance with last instance, then decrement inst_count
        // return index to original position of last instance

        float* data { static_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE)) };

        std::memmove(
            data + get_model_size() + (index * get_instance_size()),
            data + get_model_size() + (--inst_count * get_instance_size()),
            get_instance_size());

        glUnmapBuffer(GL_ARRAY_BUFFER);

        return (inst_count + 1);
    }

    void instanced_vbo::clear_instances()
    {
        inst_count = 0;
    }

    void instanced_vbo::update_instance(uint32_t index, const float* instance_data)
    {
        if (index >= inst_count) {
            return;
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glBufferSubData(GL_ARRAY_BUFFER, get_model_size() + (index * get_instance_size()), get_instance_size(), instance_data);
        
    }

} // namespace glcore