#pragma once

#include "gl_functions.hpp"

#include <array>
#include <cstdint>
#include <exception>
#include <optional>
#include <span>

namespace glcore {

enum glcore_color_format : uint8_t {
    RGB = 0b001,
    RGBA = 0b010,
    GRAYSCALE = 0b011,
};

enum glcore_color_space : uint8_t {
    LINEAR = 0b000,
    SRGB = 0b100,
};

inline std::array<std::uint8_t, 8> opengl_color_map {
    GL_RGB, GL_RGBA, GL_RED,
    0, // invalid
    GL_SRGB, GL_SRGB_ALPHA, GL_RED,
    0 // invalid
};

std::optional<int32_t> to_opengl_color(glcore_color_format format, glcore_color_space space)
{
    int32_t color = opengl_color_map[static_cast<std::uint8_t>(format) | static_cast<std::uint8_t>(space)];
    if (color == 0) {
        return std::nullopt;
    }

    return color;
}

class texture_2d {
public:
    struct texture_info_2d {
        std::uint32_t width {};
        std::uint32_t height {};
    };

    texture_2d(std::span<const float> data, texture_info_2d info, glcore_color_format format, glcore_color_space space);
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

private:
    uint32_t m_id {};
    texture_info_2d m_info {};
};

texture_2d::texture_2d(std::span<const float> data, texture_info_2d info, glcore_color_format format, glcore_color_space space)
    : m_info(info)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    auto color = to_opengl_color(format, space);

    if (!color) [[unlikely]] {
        std::terminate();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, *color, info.width, info.height, 0, *color, GL_FLOAT, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace glcore