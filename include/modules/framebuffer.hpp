/**
 * @file framebuffer.hpp
 * @author Dario Loi
 * @brief Wrapper for OpenGL Framebuffer Objects.
 *
 * @date 2023-09-18
 *
 * @copyright MIT License
 *
 * @details Wraps FBOs allowing for easy creation and usage. functionalities include
 * the internal management of renderbuffers, some utility functions to transfer
 * contents from one framebuffer to another, and some functions to bind/unbind the
 * FBO to the OpenGL context.
 */

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

#ifdef STAPLEGL_DEBUG
#include <cstdio>
#endif // STAPLEGL_DEBUG

namespace staplegl {

/**
 * @brief enum class for framebuffer attachments.
 *
 */
enum class fbo_attachment : std::uint8_t {
    NONE = 0x00,
    ATTACH_DEPTH_BUFFER = 0x01,
    ATTACH_STENCIL_BUFFER = 0x02,
    ATTACH_DEPTH_STENCIL_BUFFER = 0x03,
};

/**
 * @brief Framebuffer Object (FBO) wrapper.
 *
 */
class framebuffer {

public:
    framebuffer() noexcept;
    ~framebuffer();

    framebuffer(const framebuffer&) = delete;
    auto operator=(const framebuffer&) -> framebuffer& = delete;

    framebuffer(framebuffer&& other) noexcept;
    auto operator=(framebuffer&& other) noexcept -> framebuffer&;

    /**
     * @brief Set the renderbuffer object
     *
     * @param res The resolution of the renderbuffer object.
     * @param attachment the type of attachment (Depth buffer, Stencil Buffer, Both, None).
     *
     * @note passing staplegl::fbo_attachment::NONE results in the removal of any renderbuffer from the FBO.
     * @note this function destroys any previously held renderbuffer.
     *
     * @see staplegl::fbo_attachment
     */
    void set_renderbuffer(resolution res, fbo_attachment attachment = fbo_attachment::ATTACH_DEPTH_STENCIL_BUFFER, tex_samples samples = tex_samples::MSAA_X1);

    /**
     * @brief Set a texture as the color attachment of the framebuffer.
     *
     * @details Uses the ID contained in the `staplegl::texture_2d` object to attach it to the framebuffer,
     *
     * @note the framebuffer must be bound before calling this function.
     * @warning the framebuffer does not take ownership of the texture, hence care must be taken to ensure that the
     * texture is not destroyed before the framebuffer.
     *
     * @param tex a texture_2d to attach to the framebuffer.
     * @param index the index of the color attachment to use, used as an offset from `GL_COLOR_ATTACHMENT0`.
     */
    void set_texture(texture_2d const& tex, size_t index = 0) const;

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
    static void unbind();

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
    static auto assert_completeness() -> bool
    {
        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        return status == GL_FRAMEBUFFER_COMPLETE;
    }

    /**
     * @brief Transfer the contents of a framebuffer to another.
     *
     * @param src The source framebuffer.
     * @param dst The destination framebuffer.
     * @param res The resolution of the framebuffer.
     */
    static void transfer_data(framebuffer const& src, framebuffer const& dst, resolution res)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, src.id());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.id());

        glBlitFramebuffer(0, 0, res.width, res.height, 0, 0, res.width, res.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /**
     * @brief Get the id of the framebuffer.
     *
     * @return std::uint32_t the id of the framebuffer.
     */
    [[nodiscard]] constexpr auto id() const -> std::uint32_t
    {
        return m_id;
    }

    [[nodiscard]] constexpr auto get_renderbuffer() const -> const std::optional<renderbuffer>&;

    [[nodiscard]] constexpr auto attachment() const -> fbo_attachment
    {
        return m_attachment;
    }

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
inline framebuffer::framebuffer() noexcept
{
    glGenFramebuffers(1, &m_id);
}

inline framebuffer::~framebuffer()
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
inline framebuffer::framebuffer(framebuffer&& other) noexcept
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
inline auto framebuffer::operator=(framebuffer&& other) noexcept -> framebuffer&
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
inline void framebuffer::set_renderbuffer(resolution res, fbo_attachment attachment, tex_samples samples)
{
    if (attachment != fbo_attachment::NONE) {

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
#ifdef STAPLEGL_DEBUG
            std::fprintf(stderr, STAPLEGL_LINEINFO "invalid attachment enum %d for renderbuffer\n", static_cast<std::uint32_t>(attachment));
#endif
            std::terminate(); // crash and burn, this is unrecoverable
        }

        m_attachment = attachment; // in some cases this is a redundant operation, fair enough.
        m_renderbuffer.emplace(res, type, samples);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, static_cast<std::uint32_t>(type), GL_RENDERBUFFER, m_renderbuffer->id());
    } else {
        // unbind the old framebuffer, if present

        if (m_attachment == fbo_attachment::NONE) {
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

inline void framebuffer::set_texture(staplegl::texture_2d const& tex, size_t index) const
{

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, tex.antialias().type, tex.id(), 0);
}

inline void framebuffer::set_viewport(staplegl::resolution res)
{
    glViewport(0, 0, res.width, res.height);
}

constexpr auto framebuffer::get_renderbuffer() const -> const std::optional<renderbuffer>&
{
    return m_renderbuffer;
}

inline void framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

inline void framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace staplegl