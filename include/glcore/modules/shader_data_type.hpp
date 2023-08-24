/**
 * @file shader_data_type.hpp
 * @author Christian Panov.
 * @brief Shader data type definitions.
 *
 * @date 2023-04-28
 *
 *
 *
 * @copyright MIT License
 *
 */

#pragma once

#include "gl_functions.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>

namespace glcore::shader_data_type {

/**
 * @brief Enumerator that represents an array of a given data type.
 *
 * @details Enumerator that represents an array of a given data type. This is used to define the type of the data that is passed
 * as uniform to the shader, as well as a runtime type to feed as a parameter to other functions.
 *
 */
enum array_type_t : std::uint8_t {
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
 * @see glcore::shader_data_type::size
 * @see glcore::shader_data_type::to_opengl_type
 * @see glcore::shader_data_type::component_count
 *
 */
enum type_t : std::uint8_t {
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
 * @see glcore::shader_data_type::type_t
 * @return std::size_t the size of the shader data type in bytes.
 */
constexpr static std::size_t size(type_t type)
{
    switch (type) {
    case type_t::float32:
        return sizeof(float);
    case type_t::vec2:
        return sizeof(float) * 2;
    case type_t::vec3:
        return sizeof(float) * 3;
    case type_t::vec4:
        return sizeof(float) * 4;
    case type_t::mat3: // internally padded to use 3 vec4s
        return size(type_t::vec4) * 3;
    case type_t::mat4:
        return size(type_t::vec4) * 4;
    default:
        std::terminate();
    }
}

/**
 * @brief Convert the shader data type to an OpenGL type.
 *
 * @param type the type of the shader data.
 * @see glcore::shader_data_type::type_t
 * @return std::uint32_t the equivalent OpenGL type, as an enum.
 */
constexpr static std::uint32_t to_opengl_type(type_t type)
{
    switch (type) {
    case type_t::vec2:
        return GL_FLOAT_VEC2;
    case type_t::vec3:
        return GL_FLOAT_VEC3;
    case type_t::vec4:
        return GL_FLOAT_VEC4;
    case type_t::mat3:
        return GL_FLOAT_MAT3;
    case type_t::mat4:
        return GL_FLOAT_MAT4;
    case type_t::float32:
        return GL_FLOAT;
    default:
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
constexpr static std::uint32_t to_opengl_underlying_type(type_t type)
{
    switch (type) {
    case type_t::vec2:
    case type_t::vec3:
    case type_t::vec4:
    case type_t::mat3:
    case type_t::mat4:
    case type_t::float32:
        return GL_FLOAT;
    default:
        std::terminate();
    }
}

/**
 * @brief Get the number of components in the shader data type, useful for vector types.
 *
 * @param type the type of the shader data.
 * @see glcore::shader_data_type::type_t
 *
 * @return std::uint16_t the number of components in the shader data type.
 */
constexpr static std::uint16_t component_count(type_t type)
{
    switch (type) {
    case type_t::vec2:
        return 2;
    case type_t::vec3:
        return 3;
    case type_t::vec4:
        return 4;
    case type_t::mat3:
        return 12;
    case type_t::mat4:
        return 16;
    case type_t::float32:
        return 1;
    default:
        std::terminate();
    }
}
};