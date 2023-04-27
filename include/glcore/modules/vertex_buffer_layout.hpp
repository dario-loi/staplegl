/**
 * @file vertex_buffer_layout.hpp
 * @author Christian Panov
 * @brief Vertex Buffer Layout abstraction.
 * @version 0.1
 * @date 2023-04-28
 *
 * @details Provides a simple abstraction for vertex buffer layouts,
 * allowing to specify the layout of the vertex buffer in a simple and easy to use way.
 *
 * @copyright MIT License
 *
 */
#pragma once

#include <string_view>
#include <vector>

#include "shader_data_type.hpp"

namespace glcore {
struct vertex_attribute {
    shader_data_type::type_t type;
    std::string_view name;
    std::uint32_t offset {};
};

class vertex_buffer_layout {
public:
    vertex_buffer_layout() = default;
    vertex_buffer_layout(std::initializer_list<vertex_attribute> attributes);

    std::uint32_t stride() const;
    const std::vector<vertex_attribute>& data() const;

private:
    std::uint32_t m_stride {};
    std::vector<vertex_attribute> m_attributes;
};

vertex_buffer_layout::vertex_buffer_layout(std::initializer_list<vertex_attribute> attributes)
    : m_attributes { attributes }
{
    for (auto& [type, name, offset] : m_attributes) {
        offset = m_stride;
        m_stride += shader_data_type::size(type);
    }
}

std::uint32_t vertex_buffer_layout::stride() const
{
    return m_stride;
}

const std::vector<vertex_attribute>& vertex_buffer_layout::data() const
{
    return m_attributes;
}
}
