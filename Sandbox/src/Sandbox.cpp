#include "glcore.h"

int main()
{
	float vertices[4][7] = {
		{ -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f }, // 0
		{  0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f }, // 1
		{  0.5f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f }, // 2
		{ -0.5f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f }  // 3
	};

	unsigned int indices[2][3] = {
		{ 0, 1, 2 },
		{ 0, 2, 3 }
	};

	glcore::vertex_buffer vbo(*vertices, sizeof(vertices));
	glcore::index_buffer ibo(*indices, 6);
	glcore::vertex_buffer_layout layout =
	{
		{ glcore::shader_data_type::type::vec3, "position" },
		{ glcore::shader_data_type::type::vec4, "color" }
	};
	vbo.set_layout(layout);

	glcore::vertex_array vao;
	vao.add_vertex_buffer(vbo);
	vao.set_index_buffer(ibo);

	/*
	glcore::shader_program shaders("Basic", {
		{ glcore::shader_type::vertex, "shader_examples/vert.glsl" },
		{ glcore::shader_type::fragment, "shader_examples/frag.glsl" }
		});
	*/
	glcore::shader_program shaders("shader_examples/basic.glsl");
	shaders.bind();

	shaders.upload_uniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

	return 0;
}