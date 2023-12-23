/**
 * @file gl_functions.hpp
 * @authors Christian Panov, Dario Loi
 * @brief Loads OpenGL functions.
 *
 * @date 2023-04-28
 *
 * @details This file is a placeholder for the OpenGL function loader. You can use any loader you
 * want, as long as it is compatible with the OpenGL version you are using.
 * <br>
 * To switch to a different loader, simply replace the include directive with the one you want to
 * use. For example, if you want to use GLEW, you would replace the include directive with:
 * <br>
 * <code>#include "GL/glew.h"</code>
 * <br>
 *
 * For the examples, we are using glad.h, if you want to see the examples with a different
 * loader, you'll have to modify their loading code accordingly.
 *
 * @copyright MIT License
 *
 */

#pragma once

// very ungentelmanly use of C macros for debugging, close your eyes it'll be over soon
#ifdef STAPLEGL_DEBUG
#define STAPLEGL_STRINGIFY_DETAIL(x) #x
#define STAPLEGL_STR(x) STAPLEGL_STRINGIFY_DETAIL(x)
#define STAPLEGL_LINEINFO __FILE__ ":" STAPLEGL_STR(__LINE__)
#endif // STAPLEGL_DEBUG

/* REPLACE THIS INCLUDE DIRECTIVE WITH THE OPENGL FUNCTION LOADER YOU ARE CURRENTLY USING */
#include "glad.h"
/*----------------------------------------------------------------------------------------*/