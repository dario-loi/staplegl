#pragma once

#include "gl_functions.hpp"
#include "renderbuffer.hpp"
#include "texture.hpp"
#include "utility.hpp"

#include <array>
#include <cstdint>
#include <exception>
#include <functional>
#include <optional>
#include <span>

namespace glcore {

enum class fbo_attachment : std::uint8_t {
    NONE = 0x00,
    ATTACH_DEPTH_BUFFER = 0x01,
    ATTACH_STENCIL_BUFFER = 0x02,
    ATTACH_DEPTH_STENCIL_BUFFER = 0x03,
};



class framebuffer {

public:

    framebuffer();
    ~framebuffer();

    framebuffer(const framebuffer&) = delete;
    framebuffer& operator=(const framebuffer&) = delete;

    framebuffer(framebuffer&&) noexcept;
    framebuffer& operator=(framebuffer&&) noexcept;

    /**
     * @brief Set the renderbuffer object
     * 
     * @param res The resolution of the renderbuffer object.
     * @param attachment the type of attachment (Depth buffer, Stencil Buffer, Both, None).
     * 
     * @note passing glcore::fbo_attachment::NONE results in the removal of any renderbuffer from the FBO.
     * @note this function destroys any previously held renderbuffer.
     * 
     * @see glcore::fbo_attachment
     */
    void set_renderbuffer(resolution res, fbo_attachment attachment = fbo_attachment::ATTACH_DEPTH_STENCIL_BUFFER);

    /**
     * @brief Set a texture as the color attachment of the framebuffer.
     *
     * @details Uses the ID contained in the `glcore::texture_2d` object to attach it to the framebuffer,
     *
     * @note the framebuffer must be bound before calling this function.
     * @warning the framebuffer does not take ownership of the texture, hence care must be taken to ensure that the
     * texture is not destroyed before the framebuffer.
     *
     * @param tex a texture_2d to attach to the framebuffer.
     * @param index the index of the color attachment to use, used as an offset from `GL_COLOR_ATTACHMENT0`.
     */
    void set_texture(texture_2d const &tex, size_t index = 0) const;

    /**
     * @brief Resize the OpenGL viewport.
     * 
     * @note this does not resize held textures, but it's faster.
     * 
     * @param res The target resolution
     */
    static void set_viewport(resolution res);

    /**
     * @brief Bind the default framebuffer.
     *
     * @details Calling this function is equivalent to calling ::unbind on any framebuffer object,
     * however, it is semantically more correct to call this function when you want to bind the default
     * framebuffer (even if no framebuffer object exists).
     *
     */
    static void bind_default()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

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

    /**
     * @brief Get the id of the framebuffer.
     *
     * @return std::uint32_t the id of the framebuffer.
     */
    [[nodiscard]] std::uint32_t id() const
    {
        return m_id;
    }

    [[nodiscard]] const std::optional<renderbuffer>& get_renderbuffer() const;

private:
    std::uint32_t m_id {};
    fbo_attachment m_attachment {};
    std::optional<renderbuffer> m_renderbuffer {}; 

}; // class framebuffer

/**
 * @brief Construct a new framebuffer::framebuffer object
 *
 * @details Very thin constructor, generates an ID for the framebuffer and stores it.
 */
framebuffer::framebuffer()
{
    glGenFramebuffers(1, &m_id);
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
 * @warning the framebuffer MUST be bound before calling this function.
 */
void framebuffer::set_renderbuffer(resolution res, fbo_attachment attachment)
{
    if(attachment != fbo_attachment::NONE) {

        renderbuffer::attachment_type type {};

        switch (attachment) {
        case fbo_attachment::ATTACH_DEPTH_BUFFER:
            type = renderbuffer::attachment_type::depth;
            break;
        case fbo_attachment::ATTACH_STENCIL_BUFFER:
            type = renderbuffer::attachment_type::stencil;
            break;
        case fbo_attachment::ATTACH_DEPTH_STENCIL_BUFFER:
            type = renderbuffer::attachment_type::depth_stencil;
            break;
        default:
            std::terminate();
        }

        m_attachment = attachment; // in some cases this is a redundant operation, fair enough.
        m_renderbuffer.emplace(res, type);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, static_cast<std::uint32_t>(type), GL_RENDERBUFFER, m_renderbuffer->id());
    } else {
        // unbind the old framebuffer, if present

        if(m_attachment == fbo_attachment::NONE) {
          return; // nothing to unbind
        }

        renderbuffer::attachment_type type {};

        switch (m_attachment) {
        case fbo_attachment::ATTACH_DEPTH_BUFFER:
          type = renderbuffer::attachment_type::depth;
          break;
        case fbo_attachment::ATTACH_STENCIL_BUFFER:
          type = renderbuffer::attachment_type::stencil;
          break;
        case fbo_attachment::ATTACH_DEPTH_STENCIL_BUFFER:
          type = renderbuffer::attachment_type::depth_stencil;
          break;
          default:
            break;
          }

          m_attachment = fbo_attachment::NONE;

          glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                    static_cast<std::uint32_t>(type),
                                    GL_RENDERBUFFER, 0);
    }
}

void framebuffer::set_texture(glcore::texture_2d const& tex, size_t index) const {

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, tex.id(), 0);

}

void framebuffer::set_viewport(glcore::resolution res) {
    glViewport(0, 0, res.width, res.height);
}

const std::optional<renderbuffer>& framebuffer::get_renderbuffer() const
{
    return m_renderbuffer;
}


void framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void framebuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace glcore