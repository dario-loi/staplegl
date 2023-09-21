/**
 * @file cubemap.hpp
 * @author Dario Loi
 * @brief Cube map texture wrapper.
 * @date 2023-08-25
 * 
 * @details Abstracts away the creation and usage of a cube map texture. <br>
 * 
 * Cube maps are textures that are usually used to represent the skybox of a scene,
 * or to represent reflections. <br>
 * 
 * 
 * @copyright MIT License
 *
 */

#pragma once

#include "gl_functions.hpp"
#include "texture.hpp"
#include "utility.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

namespace staplegl {

/**
 * @brief Cube map texture wrapper.
 * 
 */
class cubemap {

public:

    /**
     * @brief Construct a new cubemap object
     * 
     * @param data The cubemap data, six faces of the cube.
     * @param res The resolution of the cubemap.
     * @param color The color format of the cubemap.
     * @param filter The filtering algorithms to use for the cubemap.
     * @param generate_mipmaps Whether to generate mipmaps for the cubemap, defaults to false.
     */
    cubemap(std::span<std::span<std::byte>, 6> data, resolution res,
        texture_color color,
        texture_filter filter,
        bool generate_mipmaps = false) noexcept;

    /**
     * @brief Destroy the cubemap object
     * 
     */
    ~cubemap() noexcept
    {
        if (m_id != 0) {
            glDeleteTextures(1, &m_id);
        }
    }

    // delete copy and copy assignment operators

    cubemap(const cubemap&) = delete;
    auto operator=(const cubemap&) -> cubemap& = delete;

    /**
     * @brief Construct a new cubemap object from another cubemap object.
     *
     * @param other the other cubemap object to move from.
     */
    cubemap(cubemap&& other) noexcept
        : m_id(other.m_id)
        , m_res(other.m_res)
        , m_color(other.m_color)
        , m_filter(other.m_filter)
    {
        other.m_id = 0;
    }

    /**
     * @brief Move assignment operator.
     *
     * @param other the other cubemap object to move from.
     * @return cubemap& a reference to this object.
     */
    auto operator=(cubemap&& other) noexcept -> cubemap&
    {
        if (this != &other) {
            m_id = other.m_id;
            m_color = other.m_color;
            m_res = other.m_res;
            m_filter = other.m_filter;
            other.m_id = 0;
        }
        return *this;
    }

    /**
     * @brief Bind the cubemap texture.
     *
     */
    void bind() const;

    /**
     * @brief Unbind the cubemap texture.
     *
     */
    static void unbind();

    /**
     * @brief Get the texture id.
     *
     * @return uint32_t the texture id.
     */
    [[nodiscard]] constexpr auto id() const noexcept -> uint32_t { return m_id; }

    /**
     * @brief Get the texture color.
     *
     * @return texture_color the texture color.
     */
    [[nodiscard]] constexpr auto color() const noexcept -> texture_color { return m_color; }

    /**
     * @brief Get the resolution of the cubemap.
     *
     * @return resolution the resolution of the cubemap.
     */
    [[nodiscard]] constexpr auto res() const noexcept -> resolution { return m_res; }

    /**
     * @brief Set the unit object to bind the texture to.
     *
     * @param unit_offset an offset that specifies the unit on which to bind the texture.
     */
    void set_unit(std::uint32_t unit_offset) const;

private:
    uint32_t m_id {};
    resolution m_res {};
    texture_color m_color {};
    texture_filter m_filter {};
};

inline cubemap::cubemap(std::span<std::span<std::byte>, 6> data, resolution res, texture_color color, texture_filter filter, bool generate_mipmaps) noexcept
    : m_res(res)
    , m_color(color)
    , m_filter(filter)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, filter.clamping);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, filter.clamping);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, filter.clamping);

    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, generate_mipmaps ? to_mipmap(filter.min_filter) : filter.min_filter);
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, filter.mag_filter);

    int i = 0;
    for (auto const& face : data) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i++, 0, m_color.internal_format, m_res.width, m_res.height, 0, m_color.format, m_color.datatype, face.data());
    }

    if (generate_mipmaps) {
        glGenerateTextureMipmap(m_id);
    }
};

inline void cubemap::bind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}

inline void cubemap::unbind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

inline void cubemap::set_unit(std::uint32_t unit_offset) const
{
    glActiveTexture(GL_TEXTURE0 + unit_offset);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}

} // namespace staplegl