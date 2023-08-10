/**
 * @file vertex_buffer_layout.hpp
 * @author Christian Panov
 * @brief Vertex Buffer Layout abstraction.
 *
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
/**
 * @brief Vertex attribute.
 *
 * @details Represents a single vertex attribute, that is, a type that
 * is used to compose a vertex buffer layout, the struct contains the type of the attribute,
 * the name of the attribute (for debugging purposes) and the offset of the attribute in the vertex buffer.
 *
 * @see shader_data_type.hpp
 */
struct vertex_attribute {
    shader_data_type::type_t type;
    std::string_view name;
    std::uint32_t offset {};
};

/**
 * @brief Vertex buffer layout.
 *
 * @details Represents a vertex buffer layout, that is, a collection of vertex attributes.
 * Through this class one can specify a layout through which a buffer's data can be
 * interpreted by OpenGL.
 *
 * @see vertex_attribute
 * @see shader_data_type
 * @see vertex_buffer.hpp
 *
 */
class vertex_buffer_layout {
public:
    vertex_buffer_layout() = default;
    /**
     * @brief Construct a new vertex buffer layout object
     *
     * @param attributes a list of vertex attributes.
     * @see vertex_attribute
     */
    vertex_buffer_layout(std::initializer_list<vertex_attribute> attributes);

    /**
     * @brief Get the stride of the vertex buffer layout.
     *
     * @return std::uint32_t, the stride of the vertex buffer layout.
     */
    std::uint32_t stride() const;

    /**
     * @brief Get the data of the vertex buffer layout.
     *
     * @return const std::vector<vertex_attribute>&, a reference to the vertex buffer layout data.
     */
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
