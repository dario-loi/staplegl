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

/**
 * @brief OpenGL texture details relating to filtering and clamping of the image
 *
 */
struct texture_filter {
    std::uint32_t min_filter {};
    std::uint32_t mag_filter {};
    std::uint32_t clamping {};
};

enum tex_samples : int32_t {

    MSAA_X1 = 1,
    MSAA_X2 = 2,
    MSAA_X4 = 4,
    MSAA_X8 = 8,
    MSAA_X16 = 16,
    MSAA_X32 = 32,

}

/**
 * @brief Convert a filter type to its mipmap counterpart.
 *
 * @note An invalid filter type will return 0.
 *
 * @param filter_type the OpenGL enum value of the filter type.
 *
 * @return constexpr std::uint32_t The OpenGL enum value of the mipmap counterpart.
 */
static constexpr std::uint32_t
to_mipmap(std::uint32_t filter_type)
{
    switch (filter_type) {
    case GL_NEAREST:
    case GL_NEAREST_MIPMAP_NEAREST:
        return GL_NEAREST_MIPMAP_NEAREST;
    case GL_LINEAR:
    case GL_LINEAR_MIPMAP_LINEAR:
        return GL_LINEAR_MIPMAP_LINEAR;
    default:
        return 0;
    }
}

class texture_2d {
public:
    /**
     * @brief Construct a new texture 2d object
     *
     * @details This constructor is only left here to allow users to create containers of texture_2d objects
     * without having to initialize them immediately, every one of those instances should be overwritten before use
     * through a call to `texture_2d::(std::span<const float> data, resolution res, texture_color color, bool generate_mipmap)`.
     *
     * A default-constructed texture_2d object is *still* valid, having an ID of 0, it is however not backed by any OpenGL texture object,
     * therefore it cannot be used in any OpenGL calls (binding it to a framebuffer, for example, would be practically equivalent to
     * unbinding the color attachment for that particular slot).
     *
     * @warning this constructor does not initialize the texture object, remember to perform initialization before using it.
     */
    texture_2d() = default;

    /**
     * @brief Construct a new texture 2d object
     *
     * @param data span of floats that represent the texture data.
     * @param res resolution of the texture.
     * @param color descriptor of the texture's color format and data type.
     * @param samples the number of samples to use for the texture, defaults to 1.
     * @param generate_mipmap whether to generate mipmaps for the texture, defaults to false.
     */
    texture_2d(std::span<const float> data, resolution res,
        texture_color color, texture_filter filters,
        tex_samples samples = tex_samples::MSAA_X1,
        bool generate_mipmap = false);

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
        , m_samples(other.m_samples)
        , m_color(other.m_color)
        , m_resolution(other.m_resolution)
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
            m_samples = other.m_samples;
            m_color = other.m_color;
            m_resolution = other.m_resolution;
            other.m_id = 0;
        }
        return *this;
    }

    /**
     * @brief Activate the texture on a unit
     *
     * @param unit_offset a texture unit offset from `GL_TEXTURE0`.
     */
    void set_unit(std::uint32_t unit_offset)
    {
        m_unit = unit_offset;
        glActiveTexture(GL_TEXTURE0 + unit_offset);
        bind();
    }

    /**
     * @brief Get the texture unit on which the texture is currently active.
     *
     * @return uint32_t a texture unit offset from `GL_TEXTURE0`.
     */
    uint32_t get_unit() const
    {
        return m_unit;
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
        glBindTexture(m_antialias.type, m_id);
    }

    /**
     * @brief Unbind the texture object.
     *
     */
    void unbind() const
    {
        glBindTexture(m_antialias.type, 0);
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
    [[nodiscard]] glcore::resolution get_resolution() const
    {
        return m_resolution;
    };

private:
    std::uint32_t m_id {};
    std::uint32_t m_unit {};
    tex_samples m_samples {};
    texture_color m_color {};
    texture_filter m_filter {};
    resolution m_resolution {};
};

texture_2d::texture_2d(std::span<const float> data, resolution res,
    texture_color color, texture_filter filter, tex_samples samples, bool generate_mipmap)
    : m_samples { m_samples }
    , m_color { color }
    , m_filter { filter }
    , m_resolution { res }
{
    glGenTextures(1, &m_id);
    glBindTexture(m_antialias.type, m_id);

    glTexParameteri(m_antialias.type, GL_TEXTURE_MIN_FILTER, generate_mipmap ? to_mipmap(filter.min_filter) : filter.min_filter);
    glTexParameteri(m_antialias.type, GL_TEXTURE_MAG_FILTER, filter.mag_filter);
    glTexParameteri(m_antialias.type, GL_TEXTURE_WRAP_S, filter.clamping);
    glTexParameteri(m_antialias.type, GL_TEXTURE_WRAP_T, filter.clamping);

    if (m_antialias.type == GL_TEXTURE_2D) {
        glTexImage2D(m_antialias.type, 0, color.internal_format, m_resolution.width, m_resolution.height, 0, color.format, color.datatype, data.data());
    } else if (m_antialias.type == GL_TEXTURE_2D_MULTISAMPLE) {
        glTexImage2DMultisample(m_antialias.type, m_antialias.samples, color.internal_format,
            m_resolution.width, m_resolution.height, GL_TRUE);
    }

    if (generate_mipmap) {
        glGenerateMipmap(m_antialias.type);
    }

    glBindTexture(m_antialias.type, 0);
}

void texture_2d::set_data(std::span<const float> data, resolution res, texture_color color, bool generate_mipmap)
{
    glTexImage2D(m_antialias.type, 0, m_color.internal_format, res.width, res.height, 0, m_color.format, m_color.datatype, data.data());
    m_color = color;
    m_resolution = res;

    if (generate_mipmap) {
        glGenerateMipmap(m_antialias.type);
    }
}

} // namespace glcore