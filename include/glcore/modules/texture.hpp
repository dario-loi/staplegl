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

#include <array>
#include <cstdint>
#include <exception>
#include <optional>
#include <span>

namespace glcore {

/**
 * @brief A struct that represents an image's dimensions.
 *
 */
struct resolution {

    std::uint32_t width {};
    std::uint32_t height {};
};

class texture_2d {
public:
    /**
     * @brief OpenGL texture details relating to the color format and data type.
     *
     */
    struct texture_info_2d {
        std::uint32_t internal_format {};
        std::uint32_t format {};
        std::uint32_t datatype {};
    };

    /**
     * @brief Construct a new texture 2d object
     *
     * @param data span of floats that represent the texture data.
     * @param res resolution of the texture.
     * @param info descriptor of the texture's color format and data type.
     * @param generate_mipmap whether to generate mipmaps for the texture.
     */
    texture_2d(std::span<const float> data, resolution res,
        texture_info_2d info = { GL_RGBA, GL_RGBA, GL_FLOAT }, bool generate_mipmap = true);

    ~texture_2d();

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
        , m_info(other.m_info)
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
            m_info = other.m_info;
            other.m_id = 0;
        }

        return *this;
    }

    /**
     * @brief Set the unit object
     *
     * @param unit_offset an offset that specifies the distance of the unit to bind
     * this texture to from the GL_TEXTURE0 unit.
     */
    void set_unit(std::uint32_t unit_offset) const
    {
        glActiveTexture(GL_TEXTURE0 + unit_offset);
    }

    void set_data(std::span<const float> data, resolution res,
        texture_info_2d info = { GL_RGBA, GL_RGBA, GL_FLOAT },
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
     * @brief Get the texture info struct.
     *
     * @return texture_info_2d
     */
    [[nodiscard]] texture_info_2d info() const
    {
        return m_info;
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

    ~texture_2d()
    {
        if (m_id != 0) {
            glDeleteTextures(1, &m_id);
        }
    }

    constexpr uint32_t id() const noexcept { return m_id; }
    constexpr texture_info_2d info() const noexcept { return m_info; }

private:
    uint32_t m_id {};
    texture_info_2d m_info {};
};

texture_2d::texture_2d(std::span<const float> data, resolution res,
    texture_info_2d info = { GL_RGBA, GL_RGBA, GL_FLOAT }, bool generate_mipmap = true)
    : m_info { info }
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexImage2D(GL_TEXTURE_2D, 0, info.internal_format, res.width, res.height, 0, info.format, info.datatype, data.data());

    if (generate_mipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_2d::set_data(std::span<const float> data, resolution res, texture_2d::texture_info_2d info = { GL_RGBA, GL_RGBA, GL_FLOAT }, bool generate_mipmap = true)
{
    glTexImage2D(GL_TEXTURE_2D, 0, m_info.internal_format, res.width, res.height, 0, m_info.format, m_info.datatype, data.data());
    m_info = info;

    if (generate_mipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

} // namespace glcore