/**
 * @file renderbuffer.hpp
 * @author Dario Loi
 * @brief Render Buffer Object (RBO) wrapper.
 * @date 2023-08-25
 *
 * @copyright MIT License
 *
 */

#pragma once

#include "gl_functions.hpp"
#include "utility.hpp"

#include <cstdint>

namespace glcore {

class renderbuffer {

public:
    enum class attachment_type : uint32_t {
        depth = GL_DEPTH_ATTACHMENT,
        stencil = GL_STENCIL_ATTACHMENT,
        depth_stencil = GL_DEPTH_STENCIL_ATTACHMENT
    };

    renderbuffer(resolution res, attachment_type type = attachment_type::depth_stencil);
    ~renderbuffer();

    renderbuffer(const renderbuffer&) = delete;
    renderbuffer& operator=(const renderbuffer&) = delete;

    renderbuffer(renderbuffer&&) noexcept;
    renderbuffer& operator=(renderbuffer&&) noexcept;

    void bind() const;
    void unbind() const;

    constexpr uint32_t id() const noexcept { return m_id; }
    constexpr resolution res() const noexcept { return m_res; }
    constexpr attachment_type type() const noexcept { return m_type; }

private:
    uint32_t m_id {};
    resolution m_res {};
    attachment_type m_type {};
};

renderbuffer::renderbuffer(resolution res, attachment_type type)
    : m_res(res)
    , m_type(type)
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
    glRenderbufferStorage(GL_RENDERBUFFER, internal_format, res.width, res.height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

renderbuffer::~renderbuffer()
{
    if (m_id != 0)
        glDeleteRenderbuffers(1, &m_id);
}

renderbuffer::renderbuffer(renderbuffer&& other) noexcept
    : m_id(other.m_id)
    , m_res(other.m_res)
    , m_type(other.m_type)
{
    other.m_id = 0;
}

renderbuffer& renderbuffer::operator=(renderbuffer&& other) noexcept
{
    if (this != &other) {
        m_id = other.m_id;
        m_res = other.m_res;
        m_type = other.m_type;

        other.m_id = 0;
    }

    return *this;
}

void renderbuffer::bind() const
{
    glBindRenderbuffer(GL_RENDERBUFFER, m_id);
}

void renderbuffer::unbind() const
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

} // namespace glcore