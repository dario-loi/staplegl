/**
 * @file shader_data_type.hpp
 * @authors Christian Panov, Dario Loi
 * @brief Shader data type definitions.
 *
 * @date 2023-04-28
 *
 * @copyright MIT License
 *
 * @details This file defines a set of enumerators that represent the data types that can be passed
 * to a shader as a uniform. These are used as runtime types that can be passed around to other
 * functions.
 *
 * @see shader.hpp
 */

#pragma once

#include "gl_functions.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>

#ifdef STAPLEGL_DEBUG
#include <cstdio>
#endif // STAPLEGL_DEBUG

namespace staplegl::shader_data_type {

/**
 * @brief Enumerator that represents an array of a given data type.
 *
 * @details Enumerator that represents an array of a given data type. This is used to define the type of the data that is passed
 * as uniform to the shader, as well as a runtime type to feed as a parameter to other functions.
 *
 */
enum class shader_array_type : std::uint8_t {
    float32_arr,
    vec2_arr,
    vec3_arr,
    vec4_arr,
    mat3_arr,
    mat4_arr,
};

/**
 * @brief Enumerator that represents a data type.
 *
 * @details Enumerator that represents a data type. This is used to define the type of the data that is passed
 * as a uniform to the shader, as well as a runtime type to feed as a parameter to other functions
 * in this module.
 *
 * @see staplegl::shader_data_type::size
 * @see staplegl::shader_data_type::to_opengl_type
 * @see staplegl::shader_data_type::component_count
 *
 */
enum class u_type : std::uint8_t {
    float32,
    vec2,
    vec3,
    vec4,
    mat3,
    mat4,
};

/**
 * @brief Get the size of the shader data type.
 *
 * @param type the type of the shader data.
 * @see staplegl::shader_data_type::u_type
 * @return std::size_t the size of the shader data type in bytes.
 */
constexpr static auto size(u_type type) -> std::size_t
{
    switch (type) {
    case u_type::float32:
        return sizeof(float);
    case u_type::vec2:
        return sizeof(float) * 2;
    case u_type::vec3:
        return sizeof(float) * 3;
    case u_type::vec4:
        return sizeof(float) * 4;
    case u_type::mat3: // internally padded to use 3 vec4s
        return size(u_type::vec4) * 3;
    case u_type::mat4:
        return size(u_type::vec4) * 4;
    default:
#ifdef STAPLEGL_DEBUG
        std::fprintf(stderr, STAPLEGL_LINEINFO ", invalid shader enum %d\n",
            static_cast<int>(type));
#endif // STAPLEGL_DEBUG
        std::terminate();
    }
}

/**
 * @brief Convert the shader data type to an OpenGL type.
 *
 * @param type the type of the shader data.
 * @see staplegl::shader_data_type::u_type
 * @return std::uint32_t the equivalent OpenGL type, as an enum.
 */
constexpr static auto to_opengl_type(u_type type) -> std::uint32_t
{
    switch (type) {
    case u_type::vec2:
        return GL_FLOAT_VEC2;
    case u_type::vec3:
        return GL_FLOAT_VEC3;
    case u_type::vec4:
        return GL_FLOAT_VEC4;
    case u_type::mat3:
        return GL_FLOAT_MAT3;
    case u_type::mat4:
        return GL_FLOAT_MAT4;
    case u_type::float32:
        return GL_FLOAT;
    default:

#ifdef STAPLEGL_DEBUG
        std::fprintf(stderr, STAPLEGL_LINEINFO ", invalid shader enum %d\n",
            static_cast<int>(type));
#endif // STAPLEGL_DEBUG
        std::terminate();
    }
}

/**
 * @brief Obtains the OpenGL type of the underlying type of the shader data type.
 *
 * @details This function retrieves the OpenGL type of the underlying type of the shader data type.
 * For scalar types, this is the same as to_opengl_type, but for vector types, this is the type of the
 * vector's components.
 *
 * @param type
 * @return constexpr std::uint32_t
 */
constexpr static auto to_opengl_underlying_type(u_type type) -> std::uint32_t
{
    switch (type) {
    case u_type::vec2:
    case u_type::vec3:
    case u_type::vec4:
    case u_type::mat3:
    case u_type::mat4:
    case u_type::float32:
        return GL_FLOAT;
    default:
#ifdef STAPLEGL_DEBUG
        std::fprintf(stderr, STAPLEGL_LINEINFO ", invalid shader enum %d\n",
            static_cast<int>(type));
#endif // STAPLEGL_DEBUG
        std::terminate();
    }
}

/**
 * @brief Get the number of components in the shader data type, useful for vector types.
 *
 * @param type the type of the shader data.
 * @see staplegl::shader_data_type::u_type
 *
 * @return std::uint16_t the number of components in the shader data type.
 */
constexpr static auto component_count(u_type type) -> std::uint16_t
{
    switch (type) {
    case u_type::vec2:
        return 2;
    case u_type::vec3:
        return 3;
    case u_type::vec4:
        return 4;
    case u_type::mat3:
        return 12;
    case u_type::mat4:
        return 16;
    case u_type::float32:
        return 1;
    default:
#ifdef STAPLEGL_DEBUG
        std::fprintf(stderr, STAPLEGL_LINEINFO ", invalid shader enum %d\n",
            static_cast<int>(type));
#endif // STAPLEGL_DEBUG
        std::terminate();
    }
}
};