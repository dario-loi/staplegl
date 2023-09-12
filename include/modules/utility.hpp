/**
 * @file utility.hpp
 * @authors Christian Panov, Dario Loi
 * @brief Utility functions for parsing files.
 *
 * @date 2023-04-28
 *
 * @copyright MIT License
 *
 */

#pragma once

#include <cstdint>
#include <fstream>
#include <string>

namespace glcore {
/**
 * @brief A struct that represents an image's dimensions.
 *
 */
struct resolution {

    std::uint32_t width {};
    std::uint32_t height {};
};
}

namespace glcore::util {

static std::string read_file(std::string_view path)
{
    std::ifstream in_file(path.data(), std::ios::ate | std::ios::binary);

    if (!in_file.is_open()) {
        return {};
    }

    size_t file_size = in_file.tellg();
    std::string result(file_size, '\0');

    in_file.seekg(0);
    in_file.read(result.data(), file_size);

    return result;
}

/**
 * @brief Get the file name object
 *
 * @param path Path to the file, relative to the current working directory.
 * @return std::string, the name of the file.
 */
static std::string get_file_name(std::string_view path)
{
    size_t last_slash = path.find_last_of("/\\");
    last_slash = (last_slash == std::string::npos) ? 0 : last_slash + 1;

    size_t last_dot = path.rfind('.');
    size_t count = (last_dot == std::string::npos) ? path.size() - last_slash : last_dot - last_slash;

    return std::string(path.substr(last_slash, count));
}
}