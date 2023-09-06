/**
 * @file texture.hpp
 * @author Dario Loi
 * @brief Texture abstraction.
 * @date 2023-08-25
 *
 * @copyright MIT License
 *
 */

#pragma once

#include "gl_functions.hpp"
#include "utility.hpp"

#include <cstdint>
#include <exception>
#include <optional>
#include <span>

namespace glcore {

/**
 * @brief OpenGL texture details relating to the color format and data type.
 *
 */
struct texture_color {
    std::uint32_t internal_format {};
    std::uint32_t format {};
    std::uint32_t datatype {};
};

class texture_2d {
public:
    /**
     * @brief Construct a new texture 2d object
     *
     * @param data span of floats that represent the texture data.
     * @param res resolution of the texture.
     * @param color descriptor of the texture's color format and data type.
     * @param generate_mipmap whether to generate mipmaps for the texture.
     */
    texture_2d(std::span<const float> data, resolution res,
        texture_color color = { GL_RGBA, GL_RGBA, GL_FLOAT }, bool generate_mipmap = true);

    ~texture_2d()
    {
        if (m_id != 0) {
            glDeleteTextures(1, &m_id);
        }
    }

    // delete copy and copy assignment operators

    texture_2d(const texture_2d&) = delete;
    texture_2d& operator=(const texture_2d&) = delete;

    /**
     * @brief Construct a new texture 2d object
     *
     * @param other the other texture_2d object to move from.
     */
    texture_2d(texture_2d&& other) noexcept
        : m_id(other.m_id)
        , m_color(other.m_color)
    {
        other.m_id = 0;
    }

    /**
     * @brief Move assignment operator.
     *
     * @param other the other texture_2d object to move from.
     * @return texture_2d& the reference to this object.
     */
    texture_2d& operator=(texture_2d&& other) noexcept
    {
        if (this != &other) {
            m_id = other.m_id;
            m_color = other.m_color;
            other.m_id = 0;
        }

        return *this;
    }

    /**
     * @brief Set the unit object
     *
     * @param unit_offset an offset that specifies the unit on which to bind the texture.
     */
    void set_unit(std::uint32_t unit_offset) const
    {
        glActiveTexture(GL_TEXTURE0 + unit_offset);
    }

    void set_data(std::span<const float> data, resolution res,
        texture_color color = { GL_RGBA, GL_RGBA, GL_FLOAT },
        bool generate_mipmap = true);

    /**
     * @brief Bind the texture object.
     *
     */
    void bind() const
    {
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    /**
     * @brief Unbind the texture object.
     *
     */
    void unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /**
     * @brief Get the texture color struct.
     *
     * @return texture_color
     */
    [[nodiscard]] texture_color color() const
    {
        return m_color;
    }

    /**
     * @brief Get the id of the texture object.
     *
     * @return std::uint32_t
     */
    [[nodiscard]] std::uint32_t id() const
    {
        return m_id;
    }

    /**
     * @brief Get the resolution of the texture object
     * 
     * @return glcore::resolution
     */
    [[nodiscard]] glcore::resolution get_resolution() const {
      return m_resolution;
    };

private:
    uint32_t m_id {};
    texture_color m_color {};
    resolution m_resolution {};
};

texture_2d::texture_2d(std::span<const float> data, resolution res,
    texture_color color, bool generate_mipmap)
    : m_color { color }
    , m_resolution { res }
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generate_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, color.internal_format, m_resolution.width, m_resolution.height, 0, color.format, color.datatype, data.data());

    if (generate_mipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_2d::set_data(std::span<const float> data, resolution res, texture_color color, bool generate_mipmap)
{
    glTexImage2D(GL_TEXTURE_2D, 0, m_color.internal_format, res.width, res.height, 0, m_color.format, m_color.datatype, data.data());
    m_color = color;
    m_resolution = res;

    if (generate_mipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

} // namespace glcore