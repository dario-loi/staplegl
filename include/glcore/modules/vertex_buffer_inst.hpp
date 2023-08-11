/**
 * @file vertex_buffer_inst.hpp
 * @author Dario Loi
 * @brief Vertex buffer object for instanced rendering.
 * @date 2023-08-10
 *
 * @copyright MIT License
 *
 */

#pragma once

#include "gl_functions.hpp"
#include "vertex_buffer.hpp"
#include "vertex_buffer_layout.hpp"

#include <cassert>
#include <numbers>
#include <span>

namespace glcore {

/**
 * @brief A vertex buffer object for instanced rendering.
 *
 * @details This class is a specialization of the vertex_buffer class, and it is meant to help with
 * instanced rendering, allowing the user of glcore to build a more performant renderer.
 *
 * When using this class, the user should initialize the buffer with an empty span of data and
 * a layout that describes how a single instance of the object is laid out in memory.
 *
 * Alternatively, the user can also supply a span with the initial data, to avoid calling
 * add_instance() multiple times.
 *
 * Adding and deleting instances automatically resizes the buffer in a way similar to std::vector,
 * so that the user does not have to worry about the size of the buffer.
 *
 * When deleting an instance, the user should be aware that the last instance in the buffer will
 * be moved to the position of the deleted instance, so the order of the instances is not preserved,
 * to help the user keep track of the indices of the instances, we return the index of the deleted
 * instance (which is the index of the last instance in the buffer before the deletion).
 *
 * @see vertex_buffer.hpp
 *
 * @attention None of the base class methods are virtual, so this class is not meant to be used polymorphically.

 */
class vertex_buffer_inst : public vertex_buffer {

private:
    /**
     * @brief The capacity of the buffer, in bytes.
     *
     */
    std::size_t m_capacity {};

    /**
     * @brief The number of instances in the buffer.
     *
     */
    std::size_t m_count {};

    /**
     * @brief Compute the new capacity of the buffer, given the current capacity.
     *
     * @param capacity the current capacity of the buffer.
     *
     * @return std::size_t the new capacity of the buffer.
     */
    constexpr std::size_t calc_capacity(std::size_t capacity) noexcept
    {
        std::size_t new_cap;

        if (capacity == 0) [[unlikely]] {
            new_cap = instance_size();
        } else if (capacity == instance_size()) [[unlikely]] {
            new_cap = instance_size() * 32;
        } else [[likely]] {
            new_cap = capacity * std::numbers::phi;
            // ceil it to the next multiple of instance_size()
        }

        return new_cap;
    }

    /**
     * @brief Resize the buffer to the given capacity.
     *
     * @param id the id of the buffer to be resized.
     *
     * @see Side effects: the old buffer will be deleted and the new buffer will be bound to GL_ARRAY_BUFFER.
     *
     * @param capacity the new capacity of the buffer, in bytes.
     */
    void resize_buffer(std::size_t old_capacity, std::size_t new_capacity) noexcept
    {
        // 1. create new buffer as big as old buffer
        // 2. copy old buffer to new buffer
        // 3. grow old buffer to new size
        // 4. copy new buffer to old buffer
        // 5. delete new buffer

        std::uint32_t new_id {};
        glGenBuffers(1, &new_id);

        glBindBuffer(GL_COPY_WRITE_BUFFER, new_id);
        glBufferData(GL_COPY_WRITE_BUFFER, old_capacity, nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_COPY_READ_BUFFER, m_id);

        // copy the data from the old buffer to the new one

        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, m_count * m_layout.stride());

        glBindBuffer(GL_COPY_WRITE_BUFFER, m_id);
        glBufferData(GL_COPY_WRITE_BUFFER, new_capacity, nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_COPY_READ_BUFFER, new_id);

        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, m_count * m_layout.stride());

        std::cout << "Resized buffer from " << old_capacity << " to " << new_capacity << std::endl;

        glDeleteBuffers(1, &new_id);
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
    }

public:
    vertex_buffer_inst(std::span<const float> instance_data,
        const vertex_buffer_layout& layout) noexcept
        : vertex_buffer { instance_data, layout, driver_draw_hint::DYNAMIC_DRAW }
        , m_capacity { instance_data.size() } {};

    vertex_buffer_inst(std::span<const float> instance_data) noexcept
        : vertex_buffer { instance_data, driver_draw_hint::DYNAMIC_DRAW }
        , m_capacity { instance_data.size() } {};

    vertex_buffer_inst(const vertex_buffer_inst&) = delete;
    vertex_buffer_inst& operator=(const vertex_buffer_inst&) = delete;

    vertex_buffer_inst(vertex_buffer_inst&&) noexcept = default;
    [[nodiscard]] vertex_buffer_inst& operator=(vertex_buffer_inst&&) noexcept = default;

    void add_instance(std::span<const float> instance_data) noexcept;
    size_t delete_instance(std::size_t index) noexcept;
    void update_instance(std::size_t index,
        std::span<const float> instance_data) noexcept;

    // UTLITIES

    constexpr std::size_t instance_count() const noexcept { return m_count; }
    constexpr std::size_t instance_size() const noexcept { return m_layout.stride(); }

}; // class vertex_buffer_inst

/*

    IMPLEMENTATIONS

*/

inline void vertex_buffer_inst::add_instance(std::span<const float> instance_data) noexcept
{
    if ((m_count + 1) * m_layout.stride() > m_capacity) {
        auto new_capacity = calc_capacity(m_capacity);
        resize_buffer(m_capacity, new_capacity);
        m_capacity = new_capacity;
    }

    update_instance(m_count, instance_data);
    m_count++;
}

void vertex_buffer_inst::update_instance(std::size_t index, std::span<const float> instance_data) noexcept
{
    assert(index < m_count || instance_data.size_bytes() == m_layout.stride());

    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferSubData(GL_ARRAY_BUFFER, index * instance_data.size_bytes(), instance_data.size_bytes(), instance_data.data());
}

inline size_t vertex_buffer_inst::delete_instance(std::size_t index) noexcept
{
    if (index >= m_count || m_count < 0) {
        return m_count;
    } // pretend we did something

    // move the last instance to the position of the deleted instance

    float* last_instance_ptr = static_cast<float*>(glMapBufferRange(
        GL_ARRAY_BUFFER, (m_count - 1) * m_layout.stride(), m_layout.stride(),
        GL_MAP_WRITE_BIT | GL_MAP_READ_BIT)); // read the last instance

    // write the last instance to the position of the deleted instance (overwriting it)

    auto last_instance = std::span<const float> { last_instance_ptr, m_layout.stride() / sizeof(float) };

    update_instance(index, last_instance);

    // we are done with the last instance, so we can unmap it
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // by reducing the count, we effectively delete the last instance (preventing duplicates)
    m_count--;

    return index;
}

} // namespace glcore
