#include <iostream>

#include "glcore/vertex_buffer.h"
#include "glcore/vertex_buffer_layout.h"
#include "glcore/index_buffer.h"
#include "glcore/vertex_array.h"
#include "glcore/shader.h"

int main()
{
	float vertices[4][2] = {
	{ -0.5f, -0.5f }, // 0
	{  0.5f, -0.5f }, // 1
	{  0.5f,  0.5f }, // 2
	{ -0.5f,  0.5f }  // 3
	};

	unsigned int indices[2][3] = {
		{ 0, 1, 2 },
		{ 0, 2, 3 }
	};

	glcore::vertex_buffer vbo(*vertices, 4 * 2 * sizeof(float));
	glcore::vertex_buffer_layout layout;
	layout.push_element<float>(2);

	glcore::vertex_array vao;
	vao.add_vertex_buffer(vbo, layout);

	glcore::index_buffer ibo(*indices, 2 * 3 * sizeof(unsigned int));

	glcore::shader_program shaders({
		{ glcore::shader_type::vertex, "shaders/vert.shader" },
		{ glcore::shader_type::fragment, "shaders/frag.shader" }
		});
	shaders.bind();

	shaders.upload_uniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

	return 0;
}