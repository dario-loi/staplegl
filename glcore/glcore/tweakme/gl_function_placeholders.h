#pragma once
#pragma warning(error:4002)

#include <cstdint>
#include <iostream>

static void __glCreateBuffers(int n, std::uint32_t* buffers) {}
static void __glDeleteBuffers(int n, const std::uint32_t* buffers) {}
static void __glBindBuffer(std::uint32_t target, std::uint32_t buffer) {}
static void __glBufferData(std::uint32_t target, std::uint32_t size, const void* data, std::uint32_t usage) {}

static void __glCreateVertexArrays(int n, std::uint32_t* arrays) {}
static void __glDeleteVertexArrays(int n, const std::uint32_t* arrays) {}
static void __glBindVertexArray(std::uint32_t array) {}
static void __glEnableVertexAttribArray(std::uint32_t index) {}
static void __glVertexAttribPointer(std::uint32_t index, int size, std::uint32_t type, unsigned char normalized, int stride, const void* pointer) {}

static std::uint32_t __glCreateProgram() {}
static std::uint32_t __glCreateShader(std::uint32_t shaderType) {}
static void __glDeleteProgram(std::uint32_t program) {}
static void __glUseProgram(std::uint32_t program) {}
static void __glLinkProgram(std::uint32_t program) {}
static void __glValidateProgram(std::uint32_t program) {}
static void __glShaderSource(std::uint32_t shader, int count, const char** string, const int* length) {}
static void __glCompileShader(std::uint32_t shader) {}
static void __glAttachShader(std::uint32_t program, std::uint32_t shader) {}
static void __glDeleteShader(std::uint32_t shader) {}
static void __glGetShaderiv(std::uint32_t shader, std::uint32_t pname, int* params) {}
static void __glGetShaderInfoLog(std::uint32_t shader, int maxLength, int* length, char* infoLog) {}

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
#define glDeleteShader(shader) __glDeleteShader(shader)
#define glGetShaderiv(shader, pname, params) __glGetShaderiv(shader, pname, params)
#define glGetShaderInfoLog(shader, maxLength, length, infoLog) __glGetShaderInfoLog(shader, maxLength, length, infoLog)

#define GL_STATIC_DRAW std::uint32_t{}
#define GL_ARRAY_BUFFER std::uint32_t{}
#define GL_ELEMENT_ARRAY_BUFFER std::uint32_t{}
#define GL_FLOAT std::uint32_t{}
#define GL_UNSIGNED_INT std::uint32_t{}
#define GL_UNSIGNED_BYTE std::uint32_t{}