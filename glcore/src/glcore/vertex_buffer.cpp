#include "vertex_buffer.h"
#include "gl_functions.h"

namespace glcore
{
	vertex_buffer::vertex_buffer(float* vertices, std::uint32_t size)
	{
		glCreateBuffers(1, &m_id);
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	vertex_buffer::vertex_buffer(float* vertices, std::uint32_t size, const vertex_buffer_layout& layout)
		: vertex_buffer{ vertices, size }
	{
		m_layout = layout;
	}

	vertex_buffer::~vertex_buffer()
	{
		glDeleteBuffers(1, &m_id);
	}

	void vertex_buffer::bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	void vertex_buffer::unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void vertex_buffer::set_layout(const vertex_buffer_layout& layout)
	{
		m_layout = layout;
	}

	const vertex_buffer_layout& vertex_buffer::layout() const
	{
		return m_layout;
	}
}