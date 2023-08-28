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

set(EXAMPLES_DIR "${PROJECT_SOURCE_DIR}/examples")
set(EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/external")

set(SANDBOX_SOURCES
    ${EXAMPLES_DIR}/sandbox.cpp
)

set(BATCHES_SOURCES
    ${EXAMPLES_DIR}/batches.cpp
)

set(GLCORE_DIR "${PROJECT_SOURCE_DIR}/include")
set(GLCORE_MODULES_DIR "${GLCORE_DIR}/modules")

set(GLCORE_HEADERS
    ${GLCORE_DIR}/glcore.hpp
    ${GLCORE_MODULES_DIR}/framebuffer.hpp
    ${GLCORE_MODULES_DIR}/gl_function_placeholders.hpp
    ${GLCORE_MODULES_DIR}/gl_functions.hpp
    ${GLCORE_MODULES_DIR}/index_buffer.hpp
    ${GLCORE_MODULES_DIR}/renderbuffer.hpp
    ${GLCORE_MODULES_DIR}/shader_data_type.hpp
    ${GLCORE_MODULES_DIR}/shader.hpp
    ${GLCORE_MODULES_DIR}/texture.hpp
    ${GLCORE_MODULES_DIR}/uniform_buffer.hpp
    ${GLCORE_MODULES_DIR}/utility.hpp
    ${GLCORE_MODULES_DIR}/vertex_array.hpp
    ${GLCORE_MODULES_DIR}/vertex_buffer.hpp
    ${GLCORE_MODULES_DIR}/vertex_buffer_inst.hpp
    ${GLCORE_MODULES_DIR}/vertex_buffer_layout.hpp
    ${GLCORE_MODULES_DIR}/cubemap.hpp
)

set(GLAD_INCLUDE_DIR "${EXTERNAL_DIR}/glad/include/glad" "${EXTERNAL_DIR}/glad/include/KHR")
set(GLAD_SOURCE ${EXTERNAL_DIR}/glad/src/glad.c)
set(GLAD_HEADERS
    ${EXTERNAL_DIR}/glad/include/glad/glad.h
    ${EXTERNAL_DIR}/glad/include/KHR/khrplatform.h
)

add_executable(sandbox ${SANDBOX_SOURCES} ${GLCORE_HEADERS} ${GLAD_HEADERS})

target_include_directories(sandbox PUBLIC
    ${GLCORE_DIR}
    ${GLCORE_MODULES_DIR}
    ${GLAD_INCLUDE_DIR}
)

add_executable(batches ${BATCHES_SOURCES} ${GLCORE_HEADERS} ${GLAD_HEADERS})

target_include_directories(batches PUBLIC
    ${GLCORE_DIR}
    ${GLCORE_MODULES_DIR}
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

# find TBB for parallelism
find_package(TBB REQUIRED)

if(NOT TBB_FOUND)
    message(ERROR " TBB not found!")
else(NOT TBB_FOUND)
    message(STATUS " TBB found")
endif(NOT TBB_FOUND)

target_link_libraries(sandbox ${LIBS})
target_include_directories(sandbox PUBLIC ${GLFW3_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

message(STATUS " LIBS: ${LIBS}")
target_link_libraries(batches ${LIBS} TBB::tbb)
target_include_directories(batches PUBLIC ${GLFW3_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

# Copy the glsl shaders to the build directory
# and add the file as a dependency to the target
set(SHADER_DIR "${PROJECT_SOURCE_DIR}/shaders")

set(EXAMPLES_SHADERS
    ${SHADER_DIR}/basic_shader.glsl
    ${SHADER_DIR}/batched_shader.glsl
    ${SHADER_DIR}/skybox_shader.glsl
    ${SHADER_DIR}/teapot_shader.glsl
)

foreach(shader ${EXAMPLES_SHADERS})
    get_filename_component(shader_name ${shader} NAME)
    configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR}/shaders/${shader_name} COPYONLY)
endforeach(shader)

set(ASSETS_DIR "${EXTERNAL_DIR}/assets")

set(SKYBOX_DIR "${ASSETS_DIR}/skybox")

foreach(face IN ITEMS right left top bottom front back)
    configure_file(${SKYBOX_DIR}/${face}.jpg ${CMAKE_CURRENT_BINARY_DIR}/assets/skybox/${face}.jpg COPYONLY)
endforeach(face)

# add warnings to executable targets
if(MSVC)
    target_compile_options(sandbox PRIVATE /W4 /WX)
    target_compile_options(batches PRIVATE /W4 /WX)
else()
    target_compile_options(sandbox PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(batches PRIVATE -Wall -Wextra -pedantic)
endif()

set(CLANG_TIDY_CHECKS
    "-*,modernize-*,performance-*,bugprone-*,clang-analyzer-*,-bugprone-easily-swappable-parameters,-bugprone-unchecked-optional-access")

# Define a custom target for clang-tidy
add_custom_target(clang-tidy-sandbox
    COMMAND clang-tidy --fix -checks=${CLANG_TIDY_CHECKS} -p ${CMAKE_BINARY_DIR} ${SANDBOX_SOURCES}
    ${SANDBOX_HEADERS} -- -I${SANDBOX_DIR}/include -I${GLAD_INCLUDE_DIR} -I${GLFW3_INCLUDE_DIR}
    -I${GLCORE_DIR} -I${GLCORE_MODULES_DIR}/include -std=c++20 -fexperimental-library
    COMMENT "Running clang-tidy on sandbox"
    VERBATIM
)

add_custom_target(clang-tidy-batches
    COMMAND clang-tidy --fix -checks=${CLANG_TIDY_CHECKS} -p ${CMAKE_BINARY_DIR} ${BATCHES_SOURCES}
    ${BATCHES_HEADERS} -- -I${BATCHES_DIR}/include -I${GLAD_INCLUDE_DIR} -I${GLFW3_INCLUDE_DIR}
    -I${GLCORE_DIR} -I${GLCORE_MODULES_DIR}/include -std=c++20 -fexperimental-library
    COMMENT "Running clang-tidy on batches"
    VERBATIM
)

set(TEAPOT_SOURCES
    ${EXAMPLES_DIR}/teapot.cpp
)

set(ASSETS_DIR "${EXTERNAL_DIR}/assets")

set(TEAPOT_DATA
    ${ASSETS_DIR}/teapot.h
)

set(GLM_DIR "${EXTERNAL_DIR}/glm")
set(STB_DIR "${EXTERNAL_DIR}/stb")

add_executable(teapot ${TEAPOT_SOURCES} ${GLCORE_HEADERS} ${GLAD_HEADERS} ${TEAPOT_DATA})
target_include_directories(teapot PUBLIC
    ${GLCORE_DIR}
    ${GLCORE_MODULES_DIR}
    ${GLAD_INCLUDE_DIR}
    ${ASSETS_DIR}
    ${GLM_DIR}
    ${STB_DIR}
)
target_link_libraries(teapot ${LIBS})
target_include_directories(teapot PUBLIC ${GLFW3_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
)
