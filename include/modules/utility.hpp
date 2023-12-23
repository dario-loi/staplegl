/**
 * @file utility.hpp
 * @authors Christian Panov, Dario Loi
 * @brief Utility functions for parsing files.
 *
 * @date 2023-04-28
 *
 * @copyright MIT License
 *
 * @details This file acts as a container for utility functions and structs that
 * are shared across the project.
 *
 */

#pragma once

#include <cstdint>
#include <fstream>
#include <string>

namespace staplegl {
/**
 * @brief A struct that represents an image's dimensions.
 *
 * @note The width and height are signed integers because OpenGL uses signed integers,
 * even though the dimensions of an image are always positive.
 */
struct resolution {

    std::int32_t width {};
    std::int32_t height {};
};

/**
 * @brief An enum that represents the number of samples for a texture.
 *
 */
enum tex_samples : int32_t {
    MSAA_X1 = 1,
    MSAA_X2 = 2,
    MSAA_X4 = 4,
    MSAA_X8 = 8,
    MSAA_X16 = 16,
    MSAA_X32 = 32,
};
} // namespace staplegl

namespace staplegl::util {

/**
 * @brief Read a file into a string.
 *
 * @warning If the file is not found, the function returns an empty string.
 *
 * @param path the file path, relative to the current working directory.
 * @return std::string the contents of the file.
 */
static auto read_file(std::string_view path) -> std::string
{
    std::ifstream in_file(path.data(), std::ios::ate | std::ios::binary);

    if (!in_file.is_open()) {
        return {};
    }

    std::ptrdiff_t const file_size = in_file.tellg();
    std::string result(file_size, '\0');

    in_file.seekg(0);
    in_file.read(result.data(), file_size);

    return result;
}

/**
 * @brief Get the filename without the extension.
 *
 * @param path Path to the file, relative to the current working directory.
 * @return std::string, the name of the file.
 */
static auto get_file_name(std::string_view path) -> std::string
{
    std::string_view basename = path.substr(path.find_last_of("/\\") + 1);

    size_t const last_dot = basename.find_last_of(".");
    return std::string(basename.substr(0, last_dot));
}
}