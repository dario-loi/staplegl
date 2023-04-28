/**
 * @file glcore.h

 * @brief GLCore, a C++17 wrapper for OpenGL.
 * @version 0.1
 * @authors Christian Panov, Dario Loi.
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
 * GLCore is a C++17 wrapper for OpenGL. It is designed with the principal goal of having
 * zero dependencies and being as lightweight as possible. You can integrate it
 * with any OpenGL project by providing your own OpenGL loader in modules/gl_functions.h,
 * and then simply importing this header file.
 *
 * GLCore offers modern RAII interfaces for OpenGL objects, such as Vertex Buffer Objects,
 * abstracting away the need to manually manage OpenGL objects, and lessening the amount
 * of boilerplate code you need to write in order to get started with OpenGL.
 *
 * @section install_sec Installation
 *
 * To install GLCore, simply clone the repository and copy the contents of the include folder
 * into your project's include folder. Then, provide your own OpenGL loader in modules/gl_functions.h.
 *
 * @section usage_sec Usage
 *
 * Each of the modules in GLCore has its own documentation page which detail its usage, you can find the documentation
 * pages in the /docs/html folder, by opening index.html.
 *
 * In general, due to GLCore being very minimalistic, its very easy to get started with it,
 * and you can find a simple example of usage in the /examples/sandbox folder, you can compile
 * this example through the CMakeLists.txt file in the root directory, but first, you need
 * to include the provided glad.h file in the /examples/sandbox/external folder, or replace it
 * with another alternative you prefer (which might require a rewrite of sandbox.cpp/CMakeLists.txt).
 *
 */