#include "shader.h"
#include "gl_functions.h"

#include <fstream>

namespace glcore
{
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
		for (const auto& [type, path] : m_shaders) {
			shader_id.push_back(compile(type, parse_code(path).data()));
		}
		for (const auto& id : shader_id) {
			glAttachShader(program, id);
		}
		link_and_validate(program);
		for (const auto& id : shader_id) {
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
			int maxLength = 0;
			std::vector<char> infoLog(maxLength);
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
			glGetShaderInfoLog(id, maxLength, &maxLength, &infoLog[0]);
			glDeleteShader(id);
			std::cout << infoLog.data() << "\n";
			return false;
		}
		return true;
	}

	std::string shader_program::parse_code(std::string_view path)
	{
		std::ifstream t(path.data());
		std::string content;
		t.seekg(0, std::ios::end);
		content.reserve(t.tellg());
		t.seekg(0, std::ios::beg);
		content.assign((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
		return content;
	}
}