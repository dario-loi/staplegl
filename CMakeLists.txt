cmake_minimum_required(VERSION 3.11)
project(sandbox)

# add modules to the cmake module path
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_COLOR_DIAGNOSTICS ON)

# And optionally
SET(CMAKE_VERBOSE_MAKEFILE ON)

set(SANDBOX_DIR "${PROJECT_SOURCE_DIR}/examples/sandbox")

set(SANDBOX_SOURCES
    ${SANDBOX_DIR}/src/sandbox.cpp
)

set(BATCHES_DIR "${PROJECT_SOURCE_DIR}/examples/batches")

set(BATCHES_SOURCES
    ${BATCHES_DIR}/src/batches.cpp
)

set(GLCORE_HEADERS
    "include/glcore/glcore.hpp"
    "include/glcore/modules/gl_functions.hpp"
    "include/glcore/modules/gl_function_placeholders.hpp"
    "include/glcore/modules/shader.hpp"
    "include/glcore/modules/utility.hpp"
    "include/glcore/modules/vertex_array.hpp"
    "include/glcore/modules/vertex_buffer_layout.hpp"
    "include/glcore/modules/vertex_buffer.hpp"
    "include/glcore/modules/index_buffer.hpp"
    "include/glcore/modules/shader_data_type.hpp"
    "include/glcore/modules/vertex_buffer_inst.hpp"
)

set(GLAD_INCLUDE_DIR "${SANDBOX_DIR}/external/glad/include/glad" "${SANDBOX_DIR}/external/glad/include/KHR")
set(GLAD_SOURCE ${SANDBOX_DIR}/external/glad/src/glad.c)
set(GLAD_HEADERS
    ${SANDBOX_DIR}/external/glad/include/glad/glad.h
    ${SANDBOX_DIR}/external/glad/include/KHR/khrplatform.h
)

add_executable(sandbox ${SANDBOX_SOURCES} ${GLCORE_HEADERS} ${GLAD_HEADERS})

target_include_directories(sandbox PUBLIC
    "${PROJECT_SOURCE_DIR}/include/glcore/modules"
    "${PROJECT_SOURCE_DIR}/include/glcore"
    ${GLAD_INCLUDE_DIR}
)

add_executable(batches ${BATCHES_SOURCES} ${GLCORE_HEADERS} ${GLAD_HEADERS})

target_include_directories(batches PUBLIC
    "${PROJECT_SOURCE_DIR}/include/glcore/modules"
    "${PROJECT_SOURCE_DIR}/include/glcore"
    ${GLAD_INCLUDE_DIR}
)

add_library(glad ${GLAD_SOURCE} ${GLAD_HEADERS})

target_include_directories(glad PUBLIC ${GLAD_INCLUDE_DIR})

# now onto the libraries
find_package(OpenGL REQUIRED)

if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
else(NOT OPENGL_FOUND)
    message(STATUS " OPENGL found at: ${OPENGL_INCLUDE_DIR} ${OPENGL_LIBRARIES}")
endif(NOT OPENGL_FOUND)

set(LIBS glad ${OPENGL_LIBRARIES})

if(APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)

    if(NOT COCOA_LIBRARY)
        message(ERROR " COCOA not found!")
    else(NOT COCOA_LIBRARY)
        message(STATUS " COCOA found at: ${COCOA_LIBRARY}")
    endif(NOT COCOA_LIBRARY)

    find_library(IOKIT_LIBRARY IOKit REQUIRED)

    if(NOT IOKIT_LIBRARY)
        message(ERROR " IOKIT not found!")
    else(NOT IOKIT_LIBRARY)
        message(STATUS " IOKIT found at: ${IOKIT_LIBRARY}")
    endif(NOT IOKIT_LIBRARY)

    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)

    if(NOT COREVIDEO_LIBRARY)
        message(ERROR " COREVIDEO not found!")
    else(NOT COREVIDEO_LIBRARY)
        message(STATUS " COREVIDEO found at: ${COREVIDEO_LIBRARY}")
    endif(NOT COREVIDEO_LIBRARY)

    if(NOT SANDBOX_PACKAGES_NOT_FOUND)
        list(APPEND LIBS ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
    endif(NOT SANDBOX_PACKAGES_NOT_FOUND)
endif(APPLE)

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)

    if(NOT X11_FOUND)
        message(ERROR " X11 not found!")
    else(NOT X11_FOUND)
        message(STATUS " X11 found at: ${X11_INCLUDE_DIR} ${X11_LIBRARIES}")
    endif(NOT X11_FOUND)

    if(NOT CMAKE_DL_LIBS)
        message(ERROR " DL not found!")
    else(NOT CMAKE_DL_LIBS)
        message(STATUS " DL found at: ${CMAKE_DL_LIBS}")
    endif(NOT CMAKE_DL_LIBS)

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED) # find out what flag is set if Threads is found.

    find_package(XRANDR REQUIRED)

    if(NOT XRANDR_LIBRARY)
        message(ERROR " XRANDR not found!")
    else(NOT XRANDR_LIBRARY)
        message(STATUS " XRANDR found at: ${XRANDR_LIBRARY}")
    endif(NOT XRANDR_LIBRARY)

    list(APPEND LIBS ${X11_LIBRARIES} ${XRANDR_LIBRARY} ${CMAKE_DL_LIBS} Threads::Threads)
endif(UNIX AND NOT APPLE)

# windows just needs the opengl32 library, which should have already been fetched.
find_package(GLFW3 REQUIRED)

if(NOT GLFW3_FOUND)
    message(ERROR " GLFW3 not found!")
else(NOT GLFW3_FOUND)
    message(STATUS " GLFW3 found at: ${GLFW3_INCLUDE_DIR} ${GLFW3_LIBRARY}")
    list(APPEND LIBS ${GLFW3_LIBRARY})
endif(NOT GLFW3_FOUND)

target_link_libraries(sandbox ${LIBS})
target_include_directories(sandbox PUBLIC ${GLFW3_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(batches ${LIBS})
target_include_directories(batches PUBLIC ${GLFW3_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

# Copy the glsl shaders to the build directory
# and add the file as a dependency to the target
set(SANDBOX_GLSL_SHADER_DIR "${SANDBOX_DIR}/shaders")

set(BATCHES_GLSL_SHADER_DIR "${BATCHES_DIR}/shaders")

set(SANDBOX_GLSL_SHADERS
    ${SANDBOX_GLSL_SHADER_DIR}/basic_shader.glsl
)

set(BATCHES_GLSL_SHADERS
    ${BATCHES_GLSL_SHADER_DIR}/batched_shader.glsl
)

foreach(shader ${SANDBOX_GLSL_SHADERS})
    get_filename_component(shader_name ${shader} NAME)
    configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR}/shaders/${shader_name} COPYONLY)
endforeach(shader)

foreach(shader ${BATCHES_GLSL_SHADERS})
    get_filename_component(shader_name ${shader} NAME)
    configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR}/shaders/${shader_name} COPYONLY)
endforeach(shader)
