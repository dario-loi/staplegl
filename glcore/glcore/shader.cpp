#include "shader.h"
#include "gl_functions.h"

#include <iostream>
#include <fstream>
#include <unordered_map>

namespace glcore
{
	shader_t::shader_t(shader_type type, std::string_view str)
		: type{ type }
		, src{ util::is_path(str.data()) ? util::read_file(str.data()) : str }
	{}

	shader_program::shader_program(std::string_view path)
		: m_shaders{ parse_shaders(util::read_file(path.data())) }
	{
		m_id = create_program();
	}

	shader_program::shader_program(std::initializer_list<shader_t> shaders)
		: m_shaders{ shaders }
	{
		m_id = create_program();
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

	void shader_program::upload_uniform1f(std::string_view name, float val)
	{
		glUniform1f(uniform_location(name), val);
	}

	void shader_program::upload_uniform2f(std::string_view name, float val0, float val1)
	{
		glUniform2f(uniform_location(name), val0, val1);
	}

	void shader_program::upload_uniform3f(std::string_view name, float val0, float val1, float val2)
	{
		glUniform3f(uniform_location(name), val0, val1, val2);
	}

	void shader_program::upload_uniform4f(std::string_view name, float val0, float val1, float val2, float val3)
	{
		glUniform4f(uniform_location(name), val0, val1, val2, val3);
	}

	std::uint32_t shader_program::program()
	{
		return m_id;
	}

	shader_t& shader_program::operator[](std::size_t index)
	{
		return m_shaders[index];
	}

	const shader_t& shader_program::operator[](std::size_t index) const
	{
		return m_shaders[index];
	}

	std::uint32_t shader_program::create_program()
	{
		std::uint32_t program = glCreateProgram();
		std::vector<std::uint32_t> shader_id;
		for (const auto& [type, src] : m_shaders) 
		{
			shader_id.push_back(compile(type, src));
		}
		for (const auto& id : shader_id) 
		{
			glAttachShader(program, id);
		}
		link_and_validate(program);
		for (const auto& id : shader_id) 
		{
			glDeleteShader(id);
		}
		return program;
	}

	std::uint32_t shader_program::compile(shader_type t_shader_type, std::string_view source)
	{
		std::uint32_t id = glCreateShader(to_gl_type(t_shader_type));
		const char* src = source.data();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);
		if (!is_valid(id)) return 0;
		return id;
	}

	std::uint32_t shader_program::to_gl_type(shader_type t_shader_type)
	{
		if (t_shader_type == shader_type::vertex)
			return GL_VERTEX_SHADER;
		else if (t_shader_type == shader_type::fragment)
			return GL_FRAGMENT_SHADER;
		else if (t_shader_type == shader_type::tesselation_control)
			return GL_TESS_CONTROL_SHADER;
		else if (t_shader_type == shader_type::tesselation_evaluation)
			return GL_TESS_EVALUATION_SHADER;
		else if (t_shader_type == shader_type::geometry)
			return GL_GEOMETRY_SHADER;
	}

	std::vector<shader_t> shader_program::parse_shaders(std::string source)
	{
		const char* type_token = "#type";
		std::vector<shader_t> shaders;
		std::unordered_map<shader_type, std::string> shader_sources;

		size_t pos = source.find(type_token, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			size_t begin = pos + strlen(type_token) + 1;
			std::string type = source.substr(begin, eol - begin);
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(type_token, nextLinePos);
			shader_sources[string_to_shader_type(type)] = (pos == std::string::npos)
				? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}
		for (const auto& [type, src] : shader_sources)
			shaders.emplace_back(type, src);

		return shaders;
	}

	int shader_program::uniform_location(std::string_view name)
	{
		int location = glGetUniformLocation(m_id, name.data());
		return location;
	}

	void shader_program::link_and_validate(std::uint32_t program)
	{
		glLinkProgram(program);
		glValidateProgram(program);
	}

	bool shader_program::is_valid(std::uint32_t id)
	{
		int is_compiled{};
		glGetShaderiv(id, GL_COMPILE_STATUS, &is_compiled);
		if (is_compiled == GL_FALSE) 
		{
			int max_length{ 0 };
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);
			std::vector<char> error_log(max_length);
			glGetShaderInfoLog(id, max_length, &max_length, &error_log[0]);
			//std::cout << error_log.data() << '\n';
			glDeleteShader(id);
			return false;
		}
		return true;
	}

	shader_type shader_program::string_to_shader_type(std::string str)
	{
		if (str == "vertex")						return shader_type::vertex;
		else if (str == "fragment")					return shader_type::fragment;
		else if (str == "tesselation_control")		return shader_type::tesselation_control;
		else if (str == "tesselation_evaluation")	return shader_type::tesselation_evaluation;
		else if (str == "geometry")					return shader_type::geometry;
	}
}