/**
 * @file vertex_buffer.hpp
 * @authors Christian Panov, Dario Loi
 * @brief Vertex Buffer Object (VBO) wrapper.
 *
 * @date 2023-04-28
 *
 * @copyright MIT License
 *
 * @details Wraps VBOs allowing for easy creation and usage. Vertex buffer objects
 * are GPU buffers that store an array of data that OpenGL uses to render primitives. <br>
 *
 * This data is stored in the GPU's memory and can be accessed by the GPU without
 * the need to send it from the CPU every time it is needed. It is stored contiguously
 * as one array of floats, hence the layout of the data must be specified to OpenGL
 * by the user through a vertex buffer layout.
 *
 * @see vertex_buffer_layout.hpp
 * @see https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
 *
 */

#pragma once
#include "gl_functions.hpp"
#include "vertex_buffer_layout.hpp"
#include <concepts>
#include <functional>
#include <span>
#include <utility>

namespace staplegl {

/**
 * @brief Concept that specifies that a type is a Plain Old Data (POD) type.
 *
 * @tparam T the type to be checked.
 */
template <typename T>
concept plain_old_data = std::is_standard_layout_v<T> && std::is_trivial_v<T>;

/**
 * @brief Enum that specifies the usage hint of a buffer.
 *
 * @details The usage hint is a hint to the driver on how the buffer will be used, it is not a guarantee
 * but setting this accordingly is likely to have positive performance implications on your
 * application.
 *
 * @see https://www.khronos.org/opengl/wiki/Buffer_Object#Usage
 *
 */
enum driver_draw_hint {
    STATIC_DRAW = GL_STATIC_DRAW,
    DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
    STREAM_DRAW = GL_STREAM_DRAW
};

/**
 * @brief Enum that specifies the access specifier of a buffer.
 *
 * @warning this is **not** a hint, although the spec does not require the driver to crash if the user
 * violates the access specifier, it is still undefined behaviour and anything goes.
 *
 */
enum driver_access_specifier {
    READ_ONLY = GL_READ_ONLY,
    WRITE_ONLY = GL_WRITE_ONLY,
    READ_WRITE = GL_READ_WRITE
};

/**
 * @brief Vertex Buffer Object (VBO) wrapper.
 *
 * @details Vertex Buffer Objects are OpenGL objects that store an array of data in the GPU's memory,
 * the data is passed as a pointer and is then accessed through a user-specified layout.
 *
 * @see https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
 * @see vertex_buffer_layout.hpp
 */
class vertex_buffer {
public:
    /**
     * @brief Construct a new vertex buffer object
     *
     * @note The vertices array is copied into the GPU's memory, so it can be safely deleted after the call.
     * @note By passing an empty std::span, the VBO will be initialized with no data.
     *
     * @param vertices a pointer to the vertices array, can be any contiguous container of floats.
     * @param size the size of the vertices array in bytes.
     */
    vertex_buffer(std::span<const float> vertices) noexcept;
    vertex_buffer(std::span<const float> vertices, driver_draw_hint hint) noexcept;
    vertex_buffer(std::span<const float> vertices, const vertex_buffer_layout& layout) noexcept;
    vertex_buffer(std::span<const float> vertices, vertex_buffer_layout layout,
        driver_draw_hint hint) noexcept;
    ~vertex_buffer();

    // delete copy and assignment, only move is allowed
    vertex_buffer(const vertex_buffer&) = delete;
    auto operator=(const vertex_buffer&) -> vertex_buffer& = delete;

    vertex_buffer(vertex_buffer&& other) noexcept;
    auto operator=(vertex_buffer&& other) noexcept -> vertex_buffer&;

    /**
     * @brief Bind the vertex buffer object.
     *
     */
    void bind() const;

    /**
     * @brief Unbind the vertex buffer object.
     *
     */
    static void unbind();

    /**
     * @brief Set the layout object
     *
     * @param layout the layout to be set.
     * @see vertex_buffer_layout.hpp
     */
    void set_layout(const vertex_buffer_layout& layout);
    [[nodiscard]] constexpr auto layout() const -> const vertex_buffer_layout&;

    /**
     * @brief Give new data to the vertex buffer object, overwriting the old one.
     *
     */

    void set_data(std::span<const float> vertices) const noexcept;

    // UTILITIES

    /**
     * @brief Get the id of the vertex buffer object.
     *
     */
    [[nodiscard]] constexpr auto id() const noexcept -> std::uint32_t { return m_id; }

    /**
     * @brief Get the size of the vertex buffer object in bytes.
     *
     */
    [[nodiscard]] constexpr auto size() const noexcept -> std::size_t { return m_layout.stride(); }

    /**
     * @brief Applies a function to the vertices of the vertex buffer object.
     *
     * @note Ensure that T is tightly packed (no padding), as the vertices of the vertex buffer object are tightly packed.
     *
     * @details This function provides an API for low-level manipulation of the vertices of the vertex buffer object,
     * this can be useful to perform a number of modifications to the vertices without issuing multiple API calls, for
     * example in the case of an instanced vertex buffer, one can update the whole buffer with a single call.
     *
     * Internally, this works by obtaining a pointer to the vertices of the vertex buffer object and reinterpreting
     * it as a user-provided type
     *
     * @param func the function to be applied to the vertices of the vertex buffer object.
     * @param access_specifier the access mode of the buffer, defaults to READ_WRITE, take care not to
     * violate the specifier (reading a write-only buffer, writing a read-only buffer, etc.) as it
     * results in undefined behaviour. If unsure, use READ_WRITE.
     * @tparam T a type that represents a vertex of the vertex buffer object.
     *
     * @see plain_old_data
     */
    template <plain_old_data T>
    void apply(const std::function<void(std::span<T> vertices)>& func, driver_access_specifier access_specifier = staplegl::READ_WRITE) noexcept;

protected:
    std::uint32_t m_id {};
    vertex_buffer_layout m_layout;
};

/*

        IMPLEMENTATIONS

*/

inline vertex_buffer::vertex_buffer(std::span<const float> vertices, vertex_buffer_layout layout,
    driver_draw_hint hint) noexcept
    : m_layout(std::move(layout))
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, static_cast<ptrdiff_t>(vertices.size_bytes()), vertices.data(), hint);
}

inline vertex_buffer::vertex_buffer(std::span<const float> vertices, driver_draw_hint hint) noexcept
    : vertex_buffer(vertices, vertex_buffer_layout {}, hint)
{
}

inline vertex_buffer::vertex_buffer(std::span<const float> vertices, const vertex_buffer_layout& layout) noexcept
    : vertex_buffer(vertices, layout, driver_draw_hint::DYNAMIC_DRAW)
{
}

inline vertex_buffer::vertex_buffer(std::span<const float> vertices) noexcept
    : vertex_buffer(vertices, vertex_buffer_layout {}, driver_draw_hint::DYNAMIC_DRAW)
{
}

inline vertex_buffer::~vertex_buffer()
{
    if (m_id != 0) {
        glDeleteBuffers(1, &m_id);
    }
}

inline vertex_buffer::vertex_buffer(vertex_buffer&& other) noexcept
    : m_id { other.m_id }
    , m_layout { std::move(other.m_layout) }
{
    other.m_id = 0;
}

inline auto vertex_buffer::operator=(vertex_buffer&& other) noexcept -> vertex_buffer&
{
    if (this != &other) {
        glDeleteBuffers(1, &m_id);
        m_id = other.m_id;
        m_layout = other.m_layout;

        other.m_id = 0;
    }

    return *this;
}

inline void vertex_buffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

inline void vertex_buffer::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

inline void vertex_buffer::set_layout(const vertex_buffer_layout& layout)
{
    m_layout = layout;
}

[[nodiscard]] constexpr auto vertex_buffer::layout() const -> const vertex_buffer_layout&
{
    return m_layout;
}

inline void vertex_buffer::set_data(std::span<const float> vertices) const noexcept
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, static_cast<ptrdiff_t>(vertices.size_bytes()), vertices.data(), GL_STATIC_DRAW);
}

template <plain_old_data T>
void vertex_buffer::apply(const std::function<void(std::span<T> vertices)>& func, driver_access_specifier access_specifier) noexcept
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);

    int32_t buffer_size {};
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);

    func(std::span { reinterpret_cast<T*>(glMapBuffer(GL_ARRAY_BUFFER, access_specifier)), // NOLINT (reinterpret-cast)
        buffer_size / sizeof(T) });

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

} // namespace staplegl