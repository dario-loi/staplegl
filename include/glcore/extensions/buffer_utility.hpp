#pragma once

#include "gl_functions.hpp"
#include <cstdint>

namespace glcore
{
    
    void move_buffer_data(uint32_t from, uint32_t to)
    {
        std::int32_t size{};
        glBindBuffer(GL_COPY_READ_BUFFER, from);
        glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);
        
        glBindBuffer(GL_COPY_WRITE_BUFFER, to);

        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
    }

} // namespace glcore
