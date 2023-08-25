/**
 * @file vertex_buffer_layout.hpp
 * @authors Christian Panov, Dario Loi
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

#include <map>
#include <ranges>
#include <string>
#include <string_view>

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
    shader_data_type::shader_type type;
    std::string name;
    std::uint32_t offset {};
    std::size_t element_count { 1 };

    vertex_attribute() = delete;
    vertex_attribute(shader_data_type::shader_type in_type, std::string_view in_name)
        : type { in_type }
        , name { in_name }
    {
    }

    vertex_attribute(shader_data_type::shader_array_type in_type, std::string_view in_name, size_t element_count)
        : type { static_cast<shader_data_type::shader_type>(in_type) }
        , name { in_name }
        , offset {}
        , element_count { element_count }
    {
    }
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
    vertex_buffer_layout(std::initializer_list<vertex_attribute> attributes)
    {
        for (const auto& attribute : attributes) {
            m_attributes.emplace(attribute.name, attribute);
        }

        for (auto& [type, name, offset, elements] : m_attributes | std::views::values) {
            offset = m_stride;
            m_stride += shader_data_type::size(type) * elements;
        }
    }

    /**
     * @brief Get the stride of the vertex buffer layout.
     *
     * @return std::size_t, the stride of the vertex buffer layout.
     */
    constexpr std::size_t stride() const noexcept { return m_stride; }

    /**
     * @brief Get the stride of the vertex buffer layout in elements (assuming float-only data).
     *
     * @return std::size_t, the number of elements in a vertex.
     */
    constexpr std::size_t stride_elements() const noexcept { return m_stride / sizeof(float); }

    /**
     * @brief Get the data of the vertex buffer layout.
     *
     * @return std::map<std::string_view, vertex_attribute>&, the data of the vertex buffer layout.
     */
    [[nodiscard]] auto get_map() const noexcept -> std::map<std::string, vertex_attribute> const&
    {
        return m_attributes;
    }

    /**
     * @brief Retrieve a specific vertex attribute by name.
     *
     * @param name the name of the vertex attribute.
     * @return vertex_attribute the vertex attribute.
     */
    [[nodiscard]] vertex_attribute operator[](std::string const& name) const noexcept
    {
        // crashes if not found, so we're still fast but at least we know if we're wrong
        return m_attributes.at(name);
    }

private:
    std::size_t m_stride {};
    std::map<std::string, vertex_attribute> m_attributes;
};

} // namespace glcore