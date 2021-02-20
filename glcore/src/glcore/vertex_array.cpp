#include "vertex_array.h"
#include "gl_functions.h"

namespace glcore
{
	vertex_array::vertex_array()
	{
		glCreateVertexArrays(1, &m_id);
	}

	vertex_array::~vertex_array()
	{
		glDeleteVertexArrays(1, &m_id);
	}

	void vertex_array::bind() const
	{
		glBindVertexArray(m_id);
	}

	void vertex_array::unbind() const
	{
		glBindVertexArray(0);
	}

	void vertex_array::add_vertex_buffer(vertex_buffer& vbo)
	{
		glBindVertexArray(m_id);
		vbo.bind();
		int attrib_index{};
		for (const auto& [type, name, offset] : vbo.layout())
		{
			glEnableVertexAttribArray(attrib_index);
			glVertexAttribPointer(
				attrib_index++, 
				shader_data_type::component_count(type),
				shader_data_type::underlying_type(type),
				GL_FALSE, 
				vbo.layout().stride(),
				reinterpret_cast<const void*>(offset)
			);
		}
		m_vertex_buffers.push_back(vbo);
	}

	void vertex_array::set_index_buffer(index_buffer& ibo)
	{
		glBindVertexArray(m_id);
		ibo.bind();
		m_index_buffer = ibo;
	}
}