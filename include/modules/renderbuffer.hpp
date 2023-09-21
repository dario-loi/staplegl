/**
 * @file renderbuffer.hpp
 * @author Dario Loi
 * @brief Render Buffer Object (RBO) wrapper.
 * @date 2023-08-25
 *
 * @copyright MIT License
 * 
 * @details Wraps RBOs allowing for easy creation and usage. Render buffer objects
 * are GPU buffers that hold screen data (similar to a texture), but they are
 * under the guarantee that the user will never need to read from them, this 
 * allows OpenGL to optimize them for rendering use (for example, by storing
 * Z-Buffer and Stencil data in a single buffer, in an efficient format).
 *
 */

#pragma once

#include "gl_functions.hpp"
#include "utility.hpp"

#include <cstdint>

namespace staplegl {

/**
 * @brief Render Buffer Object (RBO) wrapper.
 * 
 */
class renderbuffer {

public:
    /**
     * @brief Renderbuffer attachment type.
     * 
     * @details This enum is used to specify the type of attachment that the renderbuffer
     * will be used for. <br>
     * 
     */
    enum class attachment_type : uint32_t {
        depth = GL_DEPTH_ATTACHMENT,
        stencil = GL_STENCIL_ATTACHMENT,
        depth_stencil = GL_DEPTH_STENCIL_ATTACHMENT
    };

    /**
     * @brief Construct a new renderbuffer object with the specified resolution, attachment type and sample count.
     * 
     * @param res Resolution of the renderbuffer.
     * @param type Attachment type of the renderbuffer.
     * @param samples Sample count of the renderbuffer, defaults to 1.
     * 
     */
    renderbuffer(resolution res, attachment_type type = attachment_type::depth_stencil, 
        tex_samples samples = tex_samples::MSAA_X1) noexcept;

    /**
     * @brief Destroy the renderbuffer object
     * 
     */
    ~renderbuffer();

    renderbuffer(const renderbuffer&) = delete;
    auto operator=(const renderbuffer&) -> renderbuffer& = delete;

    /**
     * @brief Construct a new renderbuffer object
     *
     * @param other The renderbuffer object to move from.
     *
     * @note Moving from a renderbuffer is cheap since it only involves copying the OpenGL 
     * ID, the `other` object will be left with an ID of 0.
     *
     */
    renderbuffer(renderbuffer&& other) noexcept;

    /**
     * @brief Move assignment operator.
     * 
     * @param other The renderbuffer object to move from.
     * @return renderbuffer& The moved object.
     */
    auto operator=(renderbuffer&& other) noexcept -> renderbuffer&;

    /**
     * @brief Bind the renderbuffer object.
     * 
     */
    void bind() const;

    /**
     * @brief Unbind the renderbuffer object.
     * 
     */
    void unbind() const;

    /**
     * @brief Get the ID of the renderbuffer object.
     * 
     * @return uint32_t the ID of the renderbuffer object.
     */
    [[nodiscard]] constexpr auto id() const noexcept -> uint32_t { return m_id; }

    /**
     * @brief Get the resolution of the renderbuffer object.
     * 
     * @return resolution A resolution object containing the width and height of the renderbuffer.
     */
    [[nodiscard]] constexpr auto res() const noexcept -> resolution { return m_res; }

    /**
     * @brief Get the attachment type of the renderbuffer object.
     * 
     * 
     * @return attachment_type The attachment type of the renderbuffer object.
     */
    [[nodiscard]] constexpr auto type() const noexcept -> attachment_type { return m_type; }

    /**
     * @brief Get the sample count of the renderbuffer object.
     * 
     * @return tex_samples The sample count of the renderbuffer object.
     */
    [[nodiscard]] constexpr auto samples() const noexcept -> tex_samples { return m_samples; }

private:
    uint32_t m_id {};
    resolution m_res {};
    attachment_type m_type {};
    tex_samples m_samples {};
};

inline renderbuffer::renderbuffer(resolution res, attachment_type type, tex_samples samples) noexcept
    : m_res(res)
    , m_type(type)
    , m_samples(samples)
{

    int32_t internal_format {};

    switch (m_type) {
    case attachment_type::depth:
        internal_format = GL_DEPTH_COMPONENT24;
        break;
    case attachment_type::stencil:
        internal_format = GL_STENCIL_INDEX8;
        break;
    case attachment_type::depth_stencil:
        internal_format = GL_DEPTH24_STENCIL8;
        break;
    }

    glGenRenderbuffers(1, &m_id);
    glBindRenderbuffer(GL_RENDERBUFFER, m_id);
    if(m_samples != tex_samples::MSAA_X1) {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, static_cast<int32_t>(m_samples), internal_format, res.width, res.height);
    }
    else {
        glRenderbufferStorage(GL_RENDERBUFFER, internal_format, res.width, res.height);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

inline renderbuffer::~renderbuffer()
{
    if (m_id != 0)
        glDeleteRenderbuffers(1, &m_id);
}

inline renderbuffer::renderbuffer(renderbuffer&& other) noexcept
    : m_id(other.m_id)
    , m_res(other.m_res)
    , m_type(other.m_type)
    , m_samples(other.m_samples)
{
    other.m_id = 0;
}

inline auto renderbuffer::operator=(renderbuffer&& other) noexcept -> renderbuffer&
{
    if (this != &other) {
        m_id = other.m_id;
        m_res = other.m_res;
        m_type = other.m_type;
        m_samples = other.m_samples;

        other.m_id = 0;
    }

    return *this;
}

inline void renderbuffer::bind() const
{
    glBindRenderbuffer(GL_RENDERBUFFER, m_id);
}

inline void renderbuffer::unbind() const
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

} // namespace staplegl