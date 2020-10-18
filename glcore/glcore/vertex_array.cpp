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

	void vertex_array::add_vertex_buffer(vertex_buffer& vbo, vertex_buffer_layout& layout)
	{
		this->bind();
		vbo.bind();

		int i{}, offset{};
		for (const auto& [count, type, normalized] : layout.elements())
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i++, count, type, normalized, layout.stride(), 
				reinterpret_cast<const void*>(offset));
			offset += count * [](std::uint32_t type) 
				{ 
					if (type == (GL_FLOAT || GL_UNSIGNED_INT)) return 4;
					else if (type == GL_UNSIGNED_BYTE) return 1;
					return 0;
				}(type);
		}
	}

	void vertex_array::bind() const
	{
		glBindVertexArray(m_id);
	}

	void vertex_array::unbind() const
	{
		glBindVertexArray(0);
	}
}