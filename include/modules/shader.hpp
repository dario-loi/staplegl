/**
 * @file shader.hpp
 * @authors Christian Panov, Dario Loi
 * @brief Shader program wrapper.
 *
 * @date 2023-04-28
 *
 * @details Shader program wrapper, used to load and compile shaders, also
 * implement a very simple uniform uploading system and a simple shader parser that
 * allows the definition of multiple shaders in a single file through the use of
 * shader tags.
 *
 * @copyright MIT License
 *
 */

#pragma once

#include "gl_functions.hpp"
#include "utility.hpp"
#include <cstdint>
#include <exception>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace glcore {

/**
 * @brief The type of the shader.
 *
 * @details used to differentiate between shader types.
 *
 */
enum class shader_type {
    vertex,
    fragment,
    tess_control,
    tess_eval,
    geometry
};

/**
 * @brief Individual shader struct.
 *
 * @details The shader struct is effectively nothing more that a string that is
 * tagged with a shader type.
 *
 * @see glcore::shader_type
 *
 */
struct shader {
    shader_type type;
    std::string source;
};

/**
 * @brief Shader program class.
 *
 * @details The shader program class is used to load and compile shaders, after compilation,
 * it can also be used as an interface to each contained shader, for actions such as uploading
 * uniforms.
 *
 * It is important that uniform names are unique across all shaders in the program, otherwise
 * we risk ambiguity.
 *
 * @note if utilizing paths to load the shaders, it is important to note that they
 * will be relative to the current working directory when running the program, as an advice,
 * it is best to design your build system so that your shaders are packaged with the executable.
 *
 */
class shader_program {
public:
    shader_program() = default;

    /**
     * @brief Construct a new shader program object from a name and a path.
     *
     * @details This constructor is to be used when the shader program is to be loaded from a file
     * which contains a set of GLSL shaders, separated by the `#type` tag.
     *
     * @param name Shader program name, for debugging purposes.
     * @param path Shader program path, currently it must be relative to the current working directory.
     */
    shader_program(std::string_view name, std::string_view path);

    /**
     * @brief Construct a new shader program object from a name and a list of shaders sources.
     *
     * @details This constructor is to be used when the shader program is to be loaded from a list
     * of shader sources available at compile time, with the sources present as strings in the code.
     *
     * @param name Shader name, for debugging purposes.
     * @param shaders A list of shader sources, each shader source is a pair of shader type and shader source.
     */
    shader_program(std::string_view name, std::initializer_list<std::pair<shader_type, std::string_view>> shaders);

    /**
     * @brief Construct a new shader program object from a path.
     *
     * @details This constructor is to be used when the shader program is to be loaded from a file
     * and we do not care about the name of the shader program.
     *
     * @note This constructor automatically generates a name for the shader program by
     * obtaining it from its path.
     *
     * @param path Shader program path, currently it must be relative to the current working directory.
     */
    shader_program(std::string_view path);
    ~shader_program();

public:
    /**
     * @brief Bind the shader program.
     *
     */
    void bind() const;

    /**
     * @brief Unbind the shader program.
     *
     */
    void unbind() const;

    void upload_uniform1i(std::string_view name, int val) const;

    /**
     * @brief Upload a float uniform to the shader program.
     *
     * @param name Uniform name.
     * @param val Uniform value.
     */
    void upload_uniform1f(std::string_view name, float val) const;

    /**
     * @brief Upload a 2D float uniform to the shader program.
     *
     * @param name Uniform name.
     * @param val0 Uniform value.
     * @param val1 Uniform value.
     */
    void upload_uniform2f(std::string_view name, float val0, float val1) const;

    /**
     * @brief Upload a 3D float uniform to the shader program.
     *
     * @param name Uniform name.
     * @param val0 Uniform value.
     * @param val1 Uniform value.
     * @param val2 Uniform value.
     */
    void upload_uniform3f(std::string_view name, float val0, float val1, float val2) const;

    /**
     * @brief Upload a 4D float uniform to the shader program.
     *
     * @param name Uniform name.
     * @param val0 Uniform value.
     * @param val1 Uniform value.
     * @param val2 Uniform value.
     * @param val3 Uniform value.
     */

    void upload_uniform4f(std::string_view name, float val0, float val1, float val2, float val3) const;

    /**
     * @brief Obtain the shader program id.
     *
     * @return std::uint32_t, the shader program id in the OpenGL context.
     */
    std::uint32_t program_id() const;
    std::string_view name() const;

public:
    /**
     * @brief Obtain a reference to a shader in the shader program.
     *
     * @param index Shader index.
     * @see glcore::shader
     * @return shader&, a reference to the shader.
     */
    shader& operator[](std::size_t index);

    /**
     * @brief Obtain a const reference to a shader in the shader program.
     *
     * @param index Shader index.
     * @see glcore::shader
     * @return const shader&, a const reference to the shader.
     */
    const shader& operator[](std::size_t index) const;

private:
    /**
     * @brief Create a program object.
     *
     * @return std::uint32_t, the program object id.
     */
    std::uint32_t create_program() const;

    /**
     * @brief Create a shader object.
     *
     * @param shader_type The shader type.
     * @see glcore::shader_type
     * @param source The shader source.
     * @return std::uint32_t, the shader object id.
     */
    std::uint32_t compile(shader_type shader_type, std::string_view source) const;

    /**
     * @brief Link the shader program.
     * @details This method acts on the provided source of shaders, it will pre-process the source,
     * splitting the monolithic source into individual shader sources by scanning for the #type tag.
     *
     * @param source The shader program source.
     * @see glcore::shader_type
     *
     * @return std::vector<shader>. A vector of shaders.
     */
    [[nodiscard]] std::vector<shader> parse_shaders(const std::string& source) const;

private:
    /**
     * @brief Obtain the location of a uniform in the shader program.
     *
     * @param name Uniform name.
     * @return int, the uniform location.
     */
    int uniform_location(std::string_view name) const;

    /**
     * @brief Check if a shader program is valid.
     *
     * @param id Shader program id.
     * @return true, if the shader program is valid.
     * @return false, if the shader program is not valid.
     */
    bool is_valid(std::uint32_t id) const;

private:
    /**
     * @brief Convert a shader type to its OpenGL equivalent.
     *
     * @see glcore::shader_type
     * @param shader_type the shader type.
     * @return std::uint32_t, the OpenGL equivalent of the shader type, as an OpenGL enum.
     */
    static constexpr std::uint32_t to_gl_type(shader_type shader_type);

    /**
     * @brief Convert a string to a shader type.
     *
     * @param str The string to convert.
     *
     * @return shader_type, the shader type.
     */
    [[nodiscard]] static std::optional<shader_type> string_to_shader_type(std::string_view str);

private:
    std::uint32_t m_id {};
    std::string m_name;
    std::vector<shader> m_shaders;
};

/*

        IMPLEMENTATIONS

*/

shader_program::shader_program(std::string_view name, std::string_view path)
    : m_name { name }
    , m_shaders { parse_shaders(util::read_file(path)) }
{
    m_id = create_program();
}

shader_program::shader_program(std::string_view name,
    std::initializer_list<std::pair<shader_type, std::string_view>> shaders)
    : m_name { name }
{
    for (const auto& [type, path] : shaders)
        m_shaders.push_back({ type, util::read_file(path) });

    m_id = create_program();
}

shader_program::shader_program(std::string_view path)
    : shader_program { util::get_file_name(path), path }
{
}

shader_program::~shader_program()
{
    glDeleteProgram(m_id);
}

void shader_program::bind() const
{
    glUseProgram(m_id);
}

void shader_program::unbind() const
{
    glUseProgram(0);
}

void shader_program::upload_uniform1i(std::string_view name, int val) const
{
    glUniform1i(uniform_location(name), val);
}

void shader_program::upload_uniform1f(std::string_view name, float val) const
{
    glUniform1f(uniform_location(name), val);
}

void shader_program::upload_uniform2f(std::string_view name, float val0, float val1) const
{
    glUniform2f(uniform_location(name), val0, val1);
}

void shader_program::upload_uniform3f(std::string_view name, float val0, float val1, float val2) const
{
    glUniform3f(uniform_location(name), val0, val1, val2);
}

void shader_program::upload_uniform4f(std::string_view name, float val0, float val1, float val2, float val3) const
{
    glUniform4f(uniform_location(name), val0, val1, val2, val3);
}

std::uint32_t shader_program::program_id() const
{
    return m_id;
}

std::string_view shader_program::name() const
{
    return m_name;
}

shader& shader_program::operator[](std::size_t index)
{
    return m_shaders[index];
}

const shader& shader_program::operator[](std::size_t index) const
{
    return m_shaders[index];
}

std::uint32_t shader_program::create_program() const
{
    const std::uint32_t program { glCreateProgram() };
    std::vector<std::uint32_t> shader_ids;

    for (const auto& [type, src] : m_shaders) {
        shader_ids.push_back(compile(type, src));
    }

    for (const auto& id : shader_ids) {
        glAttachShader(program, id);
    }
    glLinkProgram(program);

    int link_success;
    glGetProgramiv(program, GL_LINK_STATUS, &link_success);

    if (!link_success) [[unlikely]] {
        int max_length {};
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
        std::vector<char> error_log(max_length);
        glGetProgramInfoLog(program, max_length, &max_length, &error_log[0]);
        std::fwrite("Failed to link shader program: ", 1, 32, stdout);
        std::fwrite(error_log.data(), error_log.size(), 1, stdout);
        std::fwrite("\n", 1, 1, stdout);
        glDeleteProgram(program);
        return 0;
    }

    glValidateProgram(program);

    int success;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (!success) [[unlikely]] {
        int max_length {};
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
        std::vector<char> error_log(max_length);
        glGetProgramInfoLog(program, max_length, &max_length, &error_log[0]);
        std::fwrite("failed to validate shader program: ", 1, 36, stdout);
        std::fwrite(error_log.data(), error_log.size(), 1, stdout);
        std::fwrite("\n", 1, 1, stdout);
        glDeleteProgram(program);
        return 0;
    }

    // Detach and delete shaders after linking the program.
    for (const auto& id : shader_ids)
        glDetachShader(program, id);
    for (const auto& id : shader_ids)
        glDeleteShader(id);

    return program;
}

std::uint32_t shader_program::compile(shader_type shader_type, std::string_view source) const
{
    const std::uint32_t id { glCreateShader(to_gl_type(shader_type)) };
    const char* src { source.data() };

    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    bool is_compiled { is_valid(id) };

    if (!is_compiled) [[unlikely]] {
        std::fwrite("Failed to compile shader: ", 1, 26, stdout);
        std::fwrite(source.data(), source.size(), 1, stdout);
        std::fwrite("\n", 1, 1, stdout);
        glDeleteShader(id);
        return 0;
    }

    return is_compiled ? id : 0;
}

std::vector<shader> shader_program::parse_shaders(const std::string& source) const
{
    std::vector<shader> shaders;
    std::string_view type_token { "#type" };

    std::size_t pos { source.find(type_token, 0) };
    while (pos != std::string::npos) {
        const std::size_t eol { source.find_first_of("\r\n", pos) };
        const std::size_t begin { pos + type_token.size() + 1 };
        const std::size_t next_line_pos { source.find_first_not_of("\r\n", eol) };
        std::string_view type { source.substr(begin, eol - begin) };

        auto shader_type { string_to_shader_type(type) };

        // With C++23 we could drastically simplify this thanks to std::optional's monadic operations.
        if (!shader_type.has_value()) [[unlikely]] {
            std::fwrite("Invalid shader type: ", 1, 21, stderr);
            std::fwrite(type.data(), type.size(), 1, stderr);
            std::fwrite("\n", 1, 1, stderr);
            std::terminate();
        }

        pos = source.find(type_token, next_line_pos);
        shaders.push_back({ shader_type.value(), pos == std::string::npos ? source.substr(next_line_pos) : source.substr(next_line_pos, pos - next_line_pos) });
    }

    return shaders;
}

int shader_program::uniform_location(std::string_view name) const
{
    static std::unordered_map<std::string_view, int> uniform_cache;

    if (uniform_cache.find(name) != uniform_cache.end()) [[likely]] {
        return uniform_cache[name];
    } else {
        const int location { glGetUniformLocation(m_id, name.data()) };
        uniform_cache[name] = location;
        return location;
    }
}

bool shader_program::is_valid(std::uint32_t id) const
{
    int is_compiled {};
    glGetShaderiv(id, GL_COMPILE_STATUS, &is_compiled);

    if (is_compiled == GL_FALSE) [[unlikely]] {
        int max_length {};
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);
        std::vector<char> error_log(max_length);
        glGetShaderInfoLog(id, max_length, &max_length, &error_log[0]);
        std::fwrite("Failed to compile shader: ", 1, 26, stdout);
        std::fwrite(error_log.data(), error_log.size(), 1, stdout);
        std::fwrite("\n", 1, 1, stdout);
        glDeleteShader(id);
        return false;
    }

    return true;
}

std::uint32_t shader_program::to_gl_type(shader_type shader_type)
{
    switch (shader_type) {
    case shader_type::vertex:
        return GL_VERTEX_SHADER;
    case shader_type::fragment:
        return GL_FRAGMENT_SHADER;
    case shader_type::tess_control:
        return GL_TESS_CONTROL_SHADER;
    case shader_type::tess_eval:
        return GL_TESS_EVALUATION_SHADER;
    case shader_type::geometry:
        return GL_GEOMETRY_SHADER;
    default:
        std::terminate();
    }
}

std::optional<shader_type> shader_program::string_to_shader_type(std::string_view str)
{

    static std::unordered_map<std::string_view, shader_type> map {
        { "vertex", shader_type::vertex },
        { "fragment", shader_type::fragment },
        { "tess_control", shader_type::tess_control },
        { "tess_eval", shader_type::tess_eval },
        { "geometry", shader_type::geometry }
    };

    if (map.find(str) != map.end()) [[likely]] {
        return map[str];
    }

    return std::nullopt;
}
}