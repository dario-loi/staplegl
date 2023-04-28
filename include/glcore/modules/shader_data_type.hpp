/**
 * @file shader_data_type.hpp
 * @author Christian Panov.
 * @brief Shader data type definitions.
 * @version 0.1
 * @date 2023-04-28
 *
 *
 *
 * @copyright MIT License
 *
 */

#pragma once

#include <cstddef>
#include <cstdint>

namespace glcore::shader_data_type {

/**
 * @brief The type of the shader data.
 *
 * @details The type of the shader data. This is used to define the type of the data that is passed
 * as a uniform to the shader, as well as a runtime type to feed as a parameter to other functions
 * in this module.
 *
 * @see glcore::shader_data_type::size
 * @see glcore::shader_data_type::to_opengl_type
 * @see glcore::shader_data_type::component_count
 *
 */
enum type_t : std::uint8_t {
    vec2,
    vec3,
    vec4
};

/**
 * @brief Get the size of the shader data type.
 *
 * @param type the type of the shader data.
 * @see glcore::shader_data_type::type_t
 * @return std::size_t the size of the shader data type in bytes.
 */
static std::size_t size(type_t type)
{
    switch (type) {
    case type_t::vec2:
        return sizeof(float) * 2;
    case type_t::vec3:
        return sizeof(float) * 3;
    case type_t::vec4:
        return sizeof(float) * 4;
    }
}

/**
 * @brief Convert the shader data type to an OpenGL type.
 *
 * @param type the type of the shader data.
 * @see glcore::shader_data_type::type_t
 * @return std::uint32_t the equivalent OpenGL type, as an enum.
 */
static std::uint32_t to_opengl_type(type_t type)
{
    switch (type) {
    case type_t::vec2:
        return 0x1406;
    case type_t::vec3:
        return 0x1406;
    case type_t::vec4:
        return 0x1406;
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
static std::uint16_t component_count(type_t type)
{
    switch (type) {
    case type_t::vec2:
        return 2;
    case type_t::vec3:
        return 3;
    case type_t::vec4:
        return 4;
    }
}
};