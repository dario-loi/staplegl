#pragma once

#include <string>
#include <vector>

namespace glcore
{
	enum class shader_type
	{
		vertex,
		fragment,
		tess_control,
		tess_eval,
		geometry
	};

	struct shader
	{
		shader_type type;
		std::string_view path;
	};

	class shader_program
	{
	public:
		shader_program() = default;
		shader_program(std::string_view name, std::string_view path);
		shader_program(std::string_view name, std::initializer_list<shader> shaders);
		shader_program(std::string_view path);
		~shader_program();

	public:
		void bind() const;
		void unbind() const;
		void upload_uniform1f(std::string_view name, float val) const;
		void upload_uniform2f(std::string_view name, float val0, float val1) const;
		void upload_uniform3f(std::string_view name, float val0, float val1, float val2) const;
		void upload_uniform4f(std::string_view name, float val0, float val1, float val2, float val3) const;
		std::uint32_t program_id() const;
		std::string_view name() const;

	public:
		shader& operator[](std::size_t index);
		const shader& operator[](std::size_t index) const;

	private:
		std::uint32_t create_program() const;
		std::uint32_t compile(shader_type t_shader_type, std::string_view source) const;
		std::vector<shader> parse_shaders(std::string_view path) const;

	private:
		int uniform_location(std::string_view name) const;
		bool is_valid(std::uint32_t id) const;

	private:
		static std::uint32_t to_gl_type(shader_type t_shader_type);
		static shader_type string_to_shader_type(std::string_view str);

	private:
		std::uint32_t m_id{};
		std::string_view m_name;
		std::vector<shader> m_shaders;
	};
}