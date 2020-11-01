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
		this->bind();
		vbo.bind();

		int attrib_index{};
		for (const auto& [data, offset] : vbo.layout())
		{
			glEnableVertexAttribArray(attrib_index);
			glVertexAttribPointer(
				attrib_index++, 
				sdt::component_count(data.type),
				sdt::underlying_type(data.type), 
				GL_FALSE, 
				vbo.layout().stride(),
				reinterpret_cast<const void*>(offset)
			);
		}
	}
}