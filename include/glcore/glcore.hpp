/**
 * @file glcore.h

 * @brief GLCore, a C++17 wrapper for OpenGL.
 * @version 0.1
 * @date 2023-04-28
 *
 * @details GLCore is a C++17 wrapper for OpenGL. It is designed to be simple and easy to use.
 * simply import this header file and provide your own OpenGL loader in modules/gl_functions.h.
 *
 *
 * @warning **IMPORTANT** Do not forget to provide an OpenGL loader in modules/gl_functions.h, or else
 * GLCore will not work.
 *
 * @see modules/gl_functions.h
 * @copyright MIT License
 *
 */

#pragma once

#include "modules/index_buffer.hpp"
#include "modules/shader.hpp"
#include "modules/vertex_array.hpp"
#include "modules/vertex_buffer.hpp"
#include "modules/vertex_buffer_layout.hpp"

/*

The rest of the file is reserved for documentation purposes. (mainly for doxygen's main page)

*/

/**
 * @mainpage GLCore
 *
 * @section intro_sec Introduction
 *
 * GLCore is a C++17 wrapper for OpenGL. It is designed to be simple and easy to use.
 * simply import this header file and provide your own OpenGL loader in modules/gl_functions.hpp.
 *
 *
 */