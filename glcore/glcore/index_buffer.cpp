#include "index_buffer.h"
#include "gl_functions.h"

namespace glcore
{
	index_buffer::index_buffer(std::uint32_t* indices, std::uint32_t size)
	{
		glCreateBuffers(1, &m_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	}

	index_buffer::~index_buffer()
	{
		glDeleteBuffers(1, &m_id);
	}

	void index_buffer::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}

	void index_buffer::unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}