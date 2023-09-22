/**
 * @file texture.hpp
 * @author Dario Loi
 * @brief Texture abstraction.
 * @date 2023-08-25
 *
 * @copyright MIT License
 *
 * @details Abstracts away the creation and usage of a 2D texture. Textures are 2D 
 * images that can be used to read and write data in the GPU. <br>
 * 
 * staplegl provides a single interface for 2D texture creation whether the texture 
 * is to be used as an object's material or as a framebuffer attachment. <br>
 * 
 * For this reason, the texture's constructor takes a number of structs that 
 * specify the texture's behavior, such as the color format and data type, the
 * various filtering algorithms to use, and the number of samples to use for the
 * texture. <br>
 * 
 * The constructor has sensible defaults, so that the user can create a simple texture 
 * just by passing the data, the resolution and the color format/filters.
 */

#pragma once

#include "gl_functions.hpp"
#include "utility.hpp"

#include <cstdint>
#include <span>

namespace staplegl {

/**
 * @brief OpenGL texture details relating to the color format and data type.
 *
 */
struct texture_color {
    std::int32_t internal_format {};
    std::uint32_t format {};
    std::uint32_t datatype {};
};

/**
 * @brief OpenGL texture details relating to filtering and clamping of the image
 *
 */
struct texture_filter {
    std::int32_t min_filter {};
    std::int32_t mag_filter {};
    std::int32_t clamping {};
};

/**
 * @brief OpenGL texture details relating to the number of samples to use for the texture.
 * 
 * @warning Antialiasing is mainly intended for use in framebuffer attachments, so 
 * if you are creating a texture to be used as an object's material, you most likely 
 * want to use `staplegl::tex_samples::MSAA_X1`.
 */
struct texture_antialias {
    std::uint32_t type {};
    tex_samples samples {};
};



/**
 * @brief Convert a filter type to its mipmap counterpart.
 *
 * @note An invalid filter type will return 0.
 *
 * @param filter_type the OpenGL enum value of the filter type.
 *
 * @return constexpr std::uint32_t The OpenGL enum value of the mipmap counterpart.
 */
static constexpr auto
to_mipmap(std::int32_t filter_type) -> std::int32_t
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

/**
 * @brief 2D texture wrapper.
 * 
 */
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
     * @param data span of floats that contains the texture data.
     * @param res resolution of the texture.
     * @param color descriptor of the texture's color format and data type.
     * @param filters descriptor of the texture's filtering and clamping.
     * @param samples the number of samples to use for the texture, defaults to 1.
     * @param generate_mipmap whether to generate mipmaps for the texture, defaults to false.
     */
    texture_2d(std::span<const float> data, resolution res,
        texture_color color, texture_filter filters,
        tex_samples samples = tex_samples::MSAA_X1,
        bool generate_mipmap = false) noexcept;

    /**
     * @brief Destroy the texture 2d object
     * 
     */
    ~texture_2d()
    {
        if (m_id != 0) {
            glDeleteTextures(1, &m_id);
        }
    }

    // delete copy and copy assignment operators

    texture_2d(const texture_2d&) = delete;
    auto operator=(const texture_2d&) -> texture_2d& = delete;



    /**
     * @brief Construct a new texture 2d object
     *
     * @param other the other texture_2d object to move from.
     */
    texture_2d(texture_2d&& other) noexcept
        : m_id(other.m_id)
        , m_unit(other.m_unit)
        , m_color(other.m_color)
        , m_resolution(other.m_resolution)
        , m_antialias(other.m_antialias)
    {
        other.m_id = 0;
    }

    /**
     * @brief Move assignment operator.
     *
     * @param other the other texture_2d object to move from.
     * @return texture_2d& the reference to this object.
     */
    auto operator=(texture_2d&& other) noexcept -> texture_2d&
    {
        if (this != &other) {
            m_id = other.m_id;
            m_unit = other.m_unit;
            m_color = other.m_color;
            m_resolution = other.m_resolution;
            m_antialias = other.m_antialias;
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
     * @brief Set the data object
     * 
     * @param data A span of floats that contains the new data.
     * @param res A resolution object containing the new resolution.
     * @param color A texture_color object containing the new color format and data type.
     * @param generate_mipmap Whether to generate mipmaps for the texture, defaults to false.
     */
    void set_data(std::span<const float> data, resolution res,
        texture_color color, bool generate_mipmap = false);

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
     * @brief Get the last texture unit this texture was bound to.
     *
     * @return uint32_t a texture unit offset from `GL_TEXTURE0`.
     */
    [[nodiscard]] auto constexpr get_unit() const -> uint32_t
    {
        return m_unit;
    }

    /**
     * @brief Get the texture color struct.
     *
     * @return texture_color
     */
    [[nodiscard]] constexpr auto color() const -> texture_color
    {
        return m_color;
    }

    /**
     * @brief Get the id of the texture object.
     *
     * @return std::uint32_t
     */
    [[nodiscard]] constexpr  auto id() const -> std::uint32_t
    {
        return m_id;
    }

    /**
     * @brief Get the resolution of the texture object
     *
     * @return staplegl::resolution
     */
    [[nodiscard]] constexpr  auto get_resolution() const -> staplegl::resolution
    {
        return m_resolution;
    };

    /**
     * @brief Get the texture filter struct.
     *
     * @return texture_filter
     */
    [[nodiscard]] constexpr  auto filter() const -> texture_filter
    {
        return m_filter;
    }

    /**
     * @brief Get the texture antialias struct.
     *
     * @return texture_antialias
     */
    [[nodiscard]] constexpr  auto antialias() const -> texture_antialias
    {
        return m_antialias;
    }



private:
    std::uint32_t m_id {};
    std::uint32_t m_unit {};
    texture_color m_color {};
    texture_filter m_filter {};
    resolution m_resolution {};
    texture_antialias m_antialias {};
};

inline texture_2d::texture_2d(std::span<const float> data, resolution res,
    texture_color color, texture_filter filter, tex_samples samples, bool generate_mipmap) noexcept
    : m_color { color }
    , m_filter { filter }
    , m_resolution { res }
    , m_antialias { (samples == tex_samples::MSAA_X1) ? texture_antialias { GL_TEXTURE_2D, samples } : texture_antialias { GL_TEXTURE_2D_MULTISAMPLE, samples } }
{
    glGenTextures(1, &m_id);
    glBindTexture(m_antialias.type, m_id);

    if (m_antialias.type == GL_TEXTURE_2D) {
        glTexParameteri(m_antialias.type, GL_TEXTURE_MIN_FILTER, generate_mipmap ? to_mipmap(filter.min_filter) : filter.min_filter);
        glTexParameteri(m_antialias.type, GL_TEXTURE_MAG_FILTER, filter.mag_filter);
        glTexParameteri(m_antialias.type, GL_TEXTURE_WRAP_S, filter.clamping);
        glTexParameteri(m_antialias.type, GL_TEXTURE_WRAP_T, filter.clamping);

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

inline void texture_2d::set_data(std::span<const float> data, resolution res, texture_color color, bool generate_mipmap)
{

    if(m_antialias.type == GL_TEXTURE_2D_MULTISAMPLE) {
      return;
    }

    glTexImage2D(m_antialias.type, 0, m_color.internal_format, res.width, res.height, 0, m_color.format, m_color.datatype, data.data());
    m_color = color;
    m_resolution = res;

    if (generate_mipmap) {
        glGenerateMipmap(m_antialias.type);
    }
}

} // namespace staplegl