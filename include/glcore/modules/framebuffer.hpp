#pragma once

#include "gl_functions.hpp"
#include "renderbuffer.hpp"
#include "texture.hpp"
#include "utility.hpp"

#include <array>
#include <cstdint>
#include <exception>
#include <optional>
#include <span>

namespace glcore {

enum framebuffer_attachment : std::uint8_t {
    NONE = 0x00,
    ATTACH_DEPTH_BUFFER = 0x01,
    ATTACH_STENCIL_BUFFER = 0x02,
    ATTACH_DEPTH_STENCIL_BUFFER = 0x03,
};

class framebuffer {

public:
    framebuffer();
    framebuffer(framebuffer_attachment attachment = NONE, resolution res = { 800, 600 });
    ~framebuffer();

    framebuffer(const framebuffer&) = delete;
    framebuffer& operator=(const framebuffer&) = delete;

    framebuffer(framebuffer&&) noexcept;
    framebuffer& operator=(framebuffer&&) noexcept;

    void add_texture(size_t index, std::span<const float> data, resolution res, texture_color info = { GL_RGBA, GL_RGBA, GL_FLOAT }, bool generate_mipmap = false);
    void add_texture(size_t index, resolution res, texture_color info = { GL_RGBA, GL_RGBA, GL_FLOAT }, bool generate_mipmap = false);
    void add_texture(size_t index, texture_2d&& texture);
    void add_renderbuffer(resolution res, framebuffer_attachment attachment = ATTACH_DEPTH_STENCIL_BUFFER);

    void bind() const;
    void unbind() const;

    /**
     * @brief Check if the framebuffer is complete.
     *
     * @details asserts whether a framebuffer is complete, e.g.: ready to be used for rendering.
     * The specifics of what makes a framebuffer complete are well-defined in the OpenGL specification
     * and available in the link below.
     *
     * @warning this function requires the framebuffer to be bound.
     *
     * @see https://www.khronos.org/opengl/wiki/Framebuffer_Object#Framebuffer_Completeness
     *
     * @return true if the framebuffer is complete.
     * @return false otherwise.
     */
    bool assert_completeness() const
    {
        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        return status == GL_FRAMEBUFFER_COMPLETE;
    }

private:
    std::uint32_t m_id {};
    framebuffer_attachment m_attachment {};
    std::array<std::optional<texture_2d>, 8> m_textures {};

    std::optional<renderbuffer> m_renderbuffer {}; // depends on the attachment type

}; // class framebuffer

/**
 * @brief Construct a new framebuffer::framebuffer object
 *
 * @param attachment Enumerator that specifies the type of the underlying renderbuffer acting as stencil buffer, depth buffer or both.
 * @param res the resolution of the renderbuffer.
 */
framebuffer::framebuffer(framebuffer_attachment attachment, resolution res)
    : m_attachment(attachment)
{
    glCreateFramebuffers(1, &m_id);

    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
    add_renderbuffer(res, m_attachment);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Construct a new framebuffer::framebuffer object without any attachments.
 *
 */
framebuffer::framebuffer()
{
    glCreateFramebuffers(1, &m_id);
}

framebuffer::~framebuffer()
{
    if (m_id != 0) {
        glDeleteFramebuffers(1, &m_id);
    }
}

/**
 * @brief Construct a new framebuffer::framebuffer object
 *
 * @param other
 */
framebuffer::framebuffer(framebuffer&& other) noexcept
    : m_id(other.m_id)
    , m_attachment(other.m_attachment)
    , m_textures(std::move(other.m_textures))
    , m_renderbuffer(std::move(other.m_renderbuffer))
{
    other.m_id = 0;
}

/**
 * @brief Move assignment operator.
 *
 * @param other the other framebuffer object to move from.
 * @return framebuffer& the reference to this object.
 */
framebuffer& framebuffer::operator=(framebuffer&& other) noexcept
{
    if (this != &other) {
        m_id = other.m_id;
        m_attachment = other.m_attachment;
        m_textures = std::move(other.m_textures);
        m_renderbuffer = std::move(other.m_renderbuffer);
        other.m_id = 0;
    }
    return *this;
}

/**
 * @brief Attach a renderbuffer to the framebuffer.
 *
 * @param res the resolution of the renderbuffer.
 * @param attachment the type of the renderbuffer.
 *
 * @warning passing a framebuffer_attachment::NONE value will result in a no-op.
 * @warning the framebuffer MUST be bound before calling this function.
 */
void framebuffer::add_renderbuffer(resolution res, framebuffer_attachment attachment)
{

    renderbuffer::attachment_type type {};

    switch (attachment) {
    case NONE:
        return;
    case ATTACH_DEPTH_BUFFER:
        type = renderbuffer::attachment_type::depth;
        break;
    case ATTACH_STENCIL_BUFFER:
        type = renderbuffer::attachment_type::stencil;
        break;
    case ATTACH_DEPTH_STENCIL_BUFFER:
        type = renderbuffer::attachment_type::depth_stencil;
        break;
    default:
        std::terminate();
    }

    m_attachment = attachment; // in some cases this is a redundant operation, fair enough.

    m_renderbuffer.emplace(res, type);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, static_cast<std::uint32_t>(attachment), GL_RENDERBUFFER, m_renderbuffer->id());
}

/**
 * @brief Add an empty (zeroed) texture object to the framebuffer.
 *
 * @param res the resolution of the texture.
 * @param info the texture info.
 * @param generate_mipmap whether to generate mipmaps for the texture.
 *
 * @warning the framebuffer MUST be bound before calling this function.
 */
void framebuffer::add_texture(size_t index, resolution res, texture_color info, bool generate_mipmap)
{
    m_textures[index].emplace(std::span<const float>(), res, info, generate_mipmap);
}

/**
 * @brief Add a texture object to the framebuffer.
 *
 * @param data the data to be uploaded to the texture.
 * @param res the resolution of the texture.
 * @param info the texture info.
 * @param generate_mipmap whether to generate mipmaps for the texture.
 *
 * @warning the framebuffer MUST be bound before calling this function.
 */
void framebuffer::add_texture(size_t index, std::span<const float> data, resolution res, texture_color info, bool generate_mipmap)
{
    m_textures[index].emplace(data, res, info, generate_mipmap);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, m_textures[index]->id(), 0);
}

} // namespace glcore