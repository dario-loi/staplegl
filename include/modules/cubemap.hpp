/**
 * @file cubemap.hpp
 * @author Dario Loi
 * @brief Cube map texture wrapper.
 * @date 2023-08-25
 *
 * @copyright MIT License
 *
 */

#pragma once

#include "gl_functions.hpp"
#include "texture.hpp"
#include "utility.hpp"

#include <array>
#include <cstdint>
#include <span>

namespace glcore {

class cubemap {

public:
    cubemap(std::span<std::span<const float>, 6>, resolution res, texture_color color = { GL_RGBA, GL_RGBA, GL_FLOAT }, bool generate_mipmaps = false);
    ~cubemap();

    // delete copy and copy assignment operators

    cubemap(const cubemap&) = delete;
    cubemap& operator=(const cubemap&) = delete;

    /**
     * @brief Construct a new cubemap object
     *
     * @param other the other cubemap object to move from.
     */
    cubemap(cubemap&& other) noexcept
        : m_id(other.m_id)
        , m_color(other.m_color)
        , m_res(other.m_res)
    {
        other.m_id = 0;
    }

    /**
     * @brief Move assignment operator.
     *
     * @param other the other cubemap object to move from.
     * @return cubemap& a reference to this object.
     */
    cubemap& operator=(cubemap&& other) noexcept
    {
        if (this != &other) {
            m_id = other.m_id;
            m_color = other.m_color;
            m_res = other.m_res;
            other.m_id = 0;
        }
        return *this;
    }

    /**
     * @brief Bind the cubemap texture.
     *
     * @param slot the texture slot to bind the texture to.
     */
    void bind() const;

    /**
     * @brief Unbind the cubemap texture.
     *
     */
    void unbind() const;

    /**
     * @brief Get the texture id.
     *
     * @return uint32_t the texture id.
     */
    [[nodiscard]] uint32_t id() const noexcept { return m_id; }

    /**
     * @brief Get the texture color.
     *
     * @return texture_color the texture color.
     */
    [[nodiscard]] texture_color color() const noexcept { return m_color; }

    /**
     * @brief Get the resolution of the cubemap.
     *
     * @return resolution the resolution of the cubemap.
     */
    [[nodiscard]] resolution res() const noexcept { return m_res; }

    /**
     * @brief Set the unit object to bind the texture to.
     *
     * @param unit_offset an offset that specifies the unit on which to bind the texture.
     */
    void set_unit(std::uint32_t unit_offset) const;

private:
    uint32_t m_id {};
    texture_color m_color {};
    resolution m_res {};
};

cubemap::cubemap(std::span<std::span<const float>, 6> data, resolution res, texture_color color, bool generate_mipmaps)
    : m_color(color)
    , m_res(res)
{
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_id);

    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, generate_mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(m_id, 1, m_color.internal_format, m_res.width, m_res.height);

    for (auto const& face : data) {
        glTextureSubImage2D(m_id, 0, 0, 0, m_res.width, m_res.height, m_color.format, m_color.datatype, face.data());
    }

    if (generate_mipmaps) {
        glGenerateTextureMipmap(m_id);
    }
};

} // namespace glcore