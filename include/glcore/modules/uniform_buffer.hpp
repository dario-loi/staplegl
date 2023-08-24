#pragma once

#include "gl_functions.hpp"
#include "vertex_buffer_layout.hpp"

#include <span>

namespace glcore {

class uniform_buffer {

    uniform_buffer(std::span<const float> contents, vertex_buffer_layout layout);
    uniform_buffer(vertex_buffer_layout layout);
    

    ~uniform_buffer();

}; // class uniform_buffer
} // namespace glcore