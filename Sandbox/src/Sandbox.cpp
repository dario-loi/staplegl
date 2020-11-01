#include <iostream>

#include "glcore/vertex_buffer.h"
#include "glcore/vertex_buffer_layout.h"
#include "glcore/index_buffer.h"
#include "glcore/vertex_array.h"
#include "glcore/shader.h"

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
	glcore::vertex_buffer_layout layout =
	{
		{{ glcore::sdt::type::vec3, "position" }},
		{{ glcore::sdt::type::vec4, "color" }}
	};
	vbo.set_layout(layout);

	glcore::vertex_array vao;
	vao.add_vertex_buffer(vbo);

	glcore::index_buffer ibo(*indices, sizeof(indices));

	glcore::shader_program shaders({
		{ glcore::shader_type::vertex, "shaders/vert.shader" },
		{ glcore::shader_type::fragment, "shaders/frag.shader" }
		});
	shaders.bind();

	shaders.upload_uniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

	return 0;
}