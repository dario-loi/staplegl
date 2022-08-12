#include "shader.h"
#include "gl_functions.h"
#include "utility.h"

#include <unordered_map>

namespace glcore
{
	shader_program::shader_program(std::string_view name, std::string_view path)
		: m_name{ name }
		, m_shaders{ parse_shaders(util::read_file(path)) }
	{
		m_id = create_program();
	}

	shader_program::shader_program(std::string_view name, std::initializer_list<shader> shaders)
		: m_name{ name }
		, m_shaders{ shaders }
	{
		m_id = create_program();
	}

	shader_program::shader_program(std::string_view path)
		: shader_program{ util::get_file_name(path), path }
	{}

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
		const std::uint32_t program = glCreateProgram();
		std::vector<std::uint32_t> shader_id;

		for (const auto& [type, source] : m_shaders)
		{
			shader_id.push_back(compile(type,
				util::is_path(source) ? util::read_file(source) : source));
		}
		for (const auto& id : shader_id) glAttachShader(program, id);
		link_and_validate(program);
		for (const auto& id : shader_id) glDetachShader(program, id);
		for (const auto& id : shader_id) glDeleteShader(id);

		return program;
	}

	std::uint32_t shader_program::compile(shader_type t_shader_type, std::string_view source) const
	{
		std::uint32_t id = glCreateShader(to_gl_type(t_shader_type));
		const char* src = source.data();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);
		return is_valid(id) ? id : 0;
	}

	std::vector<shader> shader_program::parse_shaders(std::string_view source) const
	{
		std::string_view type_token = "#type";
		std::vector<shader> shaders;
		std::unordered_map<shader_type, std::string_view> shader_sources;

		std::size_t pos = source.find(type_token, 0);
		while (pos != std::string::npos)
		{
			const std::size_t eol = source.find_first_of("\r\n", pos);
			const std::size_t begin = pos + type_token.size() + 1;
			std::string_view type = source.substr(begin, eol - begin);
			const std::size_t next_line_pos = source.find_first_not_of("\r\n", eol);

			pos = source.find(type_token, next_line_pos);
			shader_sources[string_to_shader_type(type)] = (pos == std::string::npos)
				? source.substr(next_line_pos) : source.substr(next_line_pos, pos - next_line_pos);
		}

		for (const auto& [type, source] : shader_sources) shaders.push_back({ type, source });

		return shaders;
	}

	int shader_program::uniform_location(std::string_view name) const
	{
		return glGetUniformLocation(m_id, name.data());
	}

	void shader_program::link_and_validate(std::uint32_t program) const
	{
		glLinkProgram(program);
		glValidateProgram(program);
	}

	bool shader_program::is_valid(std::uint32_t id) const
	{
		int is_compiled{};
		glGetShaderiv(id, GL_COMPILE_STATUS, &is_compiled);

		if (is_compiled == GL_FALSE)
		{
			int max_length{};
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);
			std::vector<char> error_log(max_length);
			glGetShaderInfoLog(id, max_length, &max_length, &error_log[0]);
			std::fwrite(error_log.data(), error_log.size(), 1, stdout);
			std::fwrite("\n", 1, 1, stdout);
			glDeleteShader(id);
			return false;
		}

		return true;
	}

	std::uint32_t shader_program::to_gl_type(shader_type t_shader_type)
	{
		switch (t_shader_type)
		{
		case shader_type::vertex:		return GL_VERTEX_SHADER;
		case shader_type::fragment:		return GL_FRAGMENT_SHADER;
		case shader_type::tess_control:	return GL_TESS_CONTROL_SHADER;
		case shader_type::tess_eval:	return GL_TESS_EVALUATION_SHADER;
		case shader_type::geometry:		return GL_GEOMETRY_SHADER;
		}
	}

	shader_type shader_program::string_to_shader_type(std::string_view str)
	{
		if (str == "vertex")			return shader_type::vertex;
		else if (str == "fragment")		return shader_type::fragment;
		else if (str == "tess_control")	return shader_type::tess_control;
		else if (str == "tess_eval")	return shader_type::tess_eval;
		else if (str == "geometry")		return shader_type::geometry;
	}
}