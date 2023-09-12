/**
 * @file uniform_buffer.hpp
 * @author Dario Loi
 * @brief Uniform Buffer Object (UBO) wrapper.
 * @date 2023-08-24
 *
 * @copyright MIT License
 */

#pragma once

#include "gl_functions.hpp"
#include "vertex_buffer_layout.hpp"

#include <functional>
#include <span>
#include <string_view>
#include <unordered_map>

namespace glcore {

class uniform_buffer {

public:
    uniform_buffer(std::span<const float> contents, vertex_buffer_layout const& layout, int32_t binding_point);
    uniform_buffer(vertex_buffer_layout const& layout, int32_t binding_point);

    uniform_buffer(const uniform_buffer&) = delete;
    uniform_buffer& operator=(const uniform_buffer&) = delete;

    uniform_buffer(uniform_buffer&& other) noexcept;
    [[nodiscard]] uniform_buffer& operator=(uniform_buffer&& other) noexcept;

    void bind() const;
    static void unbind();

    /**
     * @brief Set the attribute data object
     *
     * @warning This method does *NOT* bind the UBO, so you should make sure that the UBO is bound before calling this method.
     *
     * @param attribute the attribute to set the data of.
     * @param name the name of the attribute.
     */
    void set_attribute_data(std::span<const float> uniform_data, const std::string& name);

    /**
     * @brief Sets the data of an attribute of the UBO.
     *
     * @warning This method does *NOT* bind the UBO, so you should make sure that the UBO is bound before calling this method.
     *
     * @param attribute the name of the attribute to set the data of.
     * @param name the name of the attribute.
     * @param offset the offset of the data interval to set inside the attribute (always 0 for non-array attributes, can vary for
     * array attributes).
     */
    void set_attribute_data(std::span<const float> uniform_data, const std::string& name, std::size_t offset);

    void set_attribute_data(std::span<const float> uniform_data,
        size_t attribute_index);

    void set_attribute_data(std::span<const float> uniform_data,
        size_t attribute_index,
        std::size_t offset);

    ~uniform_buffer();

    // UTILITIES

    constexpr int32_t binding_point() const noexcept { return m_binding_point; }
    constexpr int32_t id() const noexcept { return m_id; }

private:
    using attr_ref = std::reference_wrapper<const vertex_attribute>;

    std::uint32_t m_id {};
    int32_t m_binding_point {};
    std::unordered_map<std::string_view, attr_ref> m_attr_cache;
    vertex_buffer_layout m_layout;

}; // class uniform_buffer

uniform_buffer::uniform_buffer(std::span<const float> contents, vertex_buffer_layout const& layout, int32_t binding_point)
    : m_binding_point { binding_point }
    , m_layout { layout }
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
    glBufferData(GL_UNIFORM_BUFFER, contents.size_bytes(), contents.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, m_binding_point, m_id);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // fill up the uniform attribute cache
    for (auto const& attr : m_layout.get_attributes()) {
        m_attr_cache.emplace(attr.name, std::cref(attr));
    }
}

uniform_buffer::uniform_buffer(vertex_buffer_layout const& layout, int32_t binding_point)
    : m_binding_point { binding_point }
    , m_layout { layout }
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
    glBufferData(GL_UNIFORM_BUFFER, layout.stride(), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, m_binding_point, m_id);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // fill up the uniform attribute cache
    for (auto const& attr : m_layout.get_attributes()) {
        m_attr_cache.emplace(attr.name, std::cref(attr));
    }
}

void uniform_buffer::bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
}

void uniform_buffer::unbind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

uniform_buffer::~uniform_buffer()
{
    if (m_id != 0) {
        glDeleteBuffers(1, &m_id);
    }
}

uniform_buffer::uniform_buffer(uniform_buffer&& other) noexcept
    : m_id { other.m_id }
    , m_binding_point { other.m_binding_point }
    , m_attr_cache { std::move(other.m_attr_cache) }
    , m_layout { std::move(other.m_layout) }
{
    other.m_id = 0;
}

[[nodiscard]] uniform_buffer& uniform_buffer::operator=(uniform_buffer&& other) noexcept
{
    if (this != &other) {
        glDeleteBuffers(1, &m_id);
        m_id = other.m_id;
        m_binding_point = other.m_binding_point;
        m_layout = std::move(other.m_layout);
        m_attr_cache = std::move(other.m_attr_cache);

        other.m_id = 0;
    }

    return *this;
}

void uniform_buffer::set_attribute_data(std::span<const float> uniform_data, const std::string& name)
{
    set_attribute_data(uniform_data, name, 0);
}

void uniform_buffer::set_attribute_data(std::span<const float> uniform_data, const std::string& name, std::size_t offset)
{
    auto const attr = m_attr_cache.at(name);

    glBufferSubData(GL_UNIFORM_BUFFER, attr.get().offset + offset * sizeof(float),
        uniform_data.size_bytes(), uniform_data.data());
}

void uniform_buffer::set_attribute_data(std::span<const float> uniform_data, size_t attribute_index)
{
    set_attribute_data(uniform_data, attribute_index, 0);
}

void uniform_buffer::set_attribute_data(std::span<const float> uniform_data, size_t attribute_index, std::size_t offset)
{
    auto const& attr = m_layout[attribute_index];

    glBufferSubData(GL_UNIFORM_BUFFER, attr.offset + offset * sizeof(float), uniform_data.size_bytes(), uniform_data.data());
}

} // namespace glcore