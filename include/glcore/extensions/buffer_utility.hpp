/**
 * @file buffer_utility.hpp
 * @author dario loi
 * @brief Small utility functions for bufffer manipulation
 *
 * @date 2023-08-7
 * 
 * @copyright MIT License
 * 
 */

#pragma once

#include "gl_functions.hpp"
#include <cstdint>

namespace glcore
{
    /**
     * @brief Copies the contents of a buffer into another buffer
     * 
     * @param from ID of the buffer to copy from
     * @param to ID of the buffer to copy to
     *
     * 
     * @note The buffers are bound as GL_COPY_READ_BUFFER and GL_COPY_WRITE_BUFFER respectively, unbinding is 
     * the responsibility of the caller (to avoid unnecessary state changes).
     */
    void move_buffer_data(uint32_t from, uint32_t to)
    {
        std::int32_t size{};
        glBindBuffer(GL_COPY_READ_BUFFER, from);
        glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);
        
        glBindBuffer(GL_COPY_WRITE_BUFFER, to);

        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
    }

} // namespace glcore
