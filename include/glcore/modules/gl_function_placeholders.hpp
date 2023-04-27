/**
 * @file gl_function_placeholders.hpp
 * @author Christian Panov
 * @brief OpenGL function placeholders.
 * @version 0.1
 * @date 2023-04-28
 *
 * @details in order for the header only library to not throw a large number of errors,
 * we need to provide placeholders for the OpenGL functions. This is done by defining
 * macros for each function, and then defining the function as an empty function (one that
 * returns a default-initialized value, or does nothing). <br>
 *
 * @note If one intends to actually use the library, they must provide their own OpenGL loader
 * and **NOT** include this header file. <br>
 *
 * @copyright MIT License
 *
 */

#pragma once
#pragma warning(error : 4002)

#include <cstdint>

static void __glCreateBuffers(int n, std::uint32_t* buffers) { }
static void __glDeleteBuffers(int n, const std::uint32_t* buffers) { }
static void __glBindBuffer(std::uint32_t target, std::uint32_t buffer) { }
static void __glBufferData(std::uint32_t target, std::uint32_t size, const void* data, std::uint32_t usage) { }

static void __glCreateVertexArrays(int n, std::uint32_t* arrays) { }
static void __glDeleteVertexArrays(int n, const std::uint32_t* arrays) { }
static void __glBindVertexArray(std::uint32_t array) { }
static void __glEnableVertexAttribArray(std::uint32_t index) { }
static void __glVertexAttribPointer(std::uint32_t index, int size, std::uint32_t type, unsigned char normalized, int stride, const void* pointer) { }

static std::uint32_t __glCreateProgram() { return std::uint32_t {}; }
static std::uint32_t __glCreateShader(std::uint32_t shaderType) { return std::uint32_t {}; }
static void __glDeleteProgram(std::uint32_t program) { }
static void __glUseProgram(std::uint32_t program) { }
static void __glLinkProgram(std::uint32_t program) { }
static void __glValidateProgram(std::uint32_t program) { }
static void __glShaderSource(std::uint32_t shader, int count, const char** string, const int* length) { }
static void __glCompileShader(std::uint32_t shader) { }
static void __glAttachShader(std::uint32_t program, std::uint32_t shader) { }
static void __glDetachShader(std::uint32_t program, std::uint32_t shader) { }
static void __glDeleteShader(std::uint32_t shader) { }
static void __glGetShaderiv(std::uint32_t shader, std::uint32_t pname, int* params) { }
static void __glGetShaderInfoLog(std::uint32_t shader, int maxLength, int* length, char* infoLog) { }
static int __glGetUniformLocation(std::uint32_t program, const char* name) { return int {}; }
static void __glUniform1f(int location, float v0) { }
static void __glUniform2f(int location, float v0, float v1) { }
static void __glUniform3f(int location, float v0, float v1, float v2) { }
static void __glUniform4f(int location, float v0, float v1, float v2, float v3) { }

#define glCreateBuffers(n, buffers) __glCreateBuffers(n, buffers)
#define glDeleteBuffers(n, buffers) __glDeleteBuffers(n, buffers)
#define glBindBuffer(target, buffer) __glBindBuffer(target, buffer)
#define glBufferData(target, size, data, usage) __glBufferData(target, size, data, usage)

#define glCreateVertexArrays(n, arrays) __glCreateVertexArrays(n, arrays)
#define glDeleteVertexArrays(n, arrays) __glDeleteVertexArrays(n, arrays)
#define glBindVertexArray(arr) __glBindVertexArray(arr)
#define glEnableVertexAttribArray(index) __glEnableVertexAttribArray(index)
#define glVertexAttribPointer(index, size, type, normalized, stride, pointer) __glVertexAttribPointer(index, size, type, normalized, stride, pointer)

#define glCreateProgram() __glCreateProgram()
#define glCreateShader(shaderType) __glCreateShader(shaderType)
#define glDeleteProgram(program) __glDeleteProgram(program)
#define glUseProgram(program) __glUseProgram(program)
#define glLinkProgram(program) __glLinkProgram(program)
#define glValidateProgram(program) __glValidateProgram(program)
#define glShaderSource(shader, count, string, length) __glShaderSource(shader, count, string, length)
#define glCompileShader(shader) __glCompileShader(shader)
#define glAttachShader(program, shader) __glAttachShader(program, shader)
#define glDetachShader(program, shader) __glDetachShader(program, shader)
#define glDeleteShader(shader) __glDeleteShader(shader)
#define glGetShaderiv(shader, pname, params) __glGetShaderiv(shader, pname, params)
#define glGetShaderInfoLog(shader, maxLength, length, infoLog) __glGetShaderInfoLog(shader, maxLength, length, infoLog)
#define glGetUniformLocation(program, name) __glGetUniformLocation(program, name)
#define glUniform1f(location, v0) __glUniform1f(location, v0)
#define glUniform2f(location, v0, v1) __glUniform2f(location, v0, v1)
#define glUniform3f(location, v0, v1, v2) __glUniform3f(location, v0, v1, v2)
#define glUniform4f(location, v0, v1, v2, v3) __glUniform4f(location, v0, v1, v2, v3)

#define GL_STATIC_DRAW \
    std::uint32_t      \
    {                  \
    }
#define GL_ARRAY_BUFFER \
    std::uint32_t       \
    {                   \
    }
#define GL_ELEMENT_ARRAY_BUFFER \
    std::uint32_t               \
    {                           \
    }
#define GL_VERTEX_SHADER \
    std::uint32_t        \
    {                    \
    }
#define GL_FRAGMENT_SHADER \
    std::uint32_t          \
    {                      \
    }
#define GL_TESS_CONTROL_SHADER \
    std::uint32_t              \
    {                          \
    }
#define GL_TESS_EVALUATION_SHADER \
    std::uint32_t                 \
    {                             \
    }
#define GL_GEOMETRY_SHADER \
    std::uint32_t          \
    {                      \
    }
#define GL_COMPILE_STATUS \
    std::uint32_t         \
    {                     \
    }
#define GL_INFO_LOG_LENGTH \
    std::uint32_t          \
    {                      \
    }
#define GL_FLOAT  \
    std::uint32_t \
    {             \
    }
#define GL_UNSIGNED_INT \
    std::uint32_t       \
    {                   \
    }
#define GL_UNSIGNED_BYTE \
    std::uint32_t        \
    {                    \
    }
#define GL_FALSE 0