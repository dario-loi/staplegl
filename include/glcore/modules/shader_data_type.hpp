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
enum type_t : std::uint8_t {
    vec2,
    vec3,
    vec4
};

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