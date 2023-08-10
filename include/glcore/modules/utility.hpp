/**
 * @file utility.hpp
 * @author Christian Panov
 * @brief Utility functions for parsing files.
 *
 * @date 2023-04-28
 *
 * @copyright MIT License
 *
 */

#pragma once

#include <fstream>
#include <string>

namespace glcore::util {
/**
 * @brief Reads a file into a string.
 *
 *
 * @param path Path to the file, relative to the current working directory.
 * @return std::string, the contents of the file.
 */
static std::string read_file(std::string_view path)
{
    std::string result;

    if (std::ifstream in(path.data(), std::ios::in); in) {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(result.data(), result.size());
        in.close();
    }

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
    auto last_slash = path.find_last_of("/\\");
    last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;

    const auto last_dot = path.rfind('.');
    const auto count = last_dot == std::string::npos ? path.size() - last_slash : last_dot - last_slash;

    return path.substr(last_slash, count).data();
}
}