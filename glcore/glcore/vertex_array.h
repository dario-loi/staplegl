#pragma once

#include "gl_object.h"
#include "vertex_buffer.h"
#include "index_buffer.h"

namespace glcore
{
	class vertex_array : public gl_object
	{
	public:
		vertex_array();
		~vertex_array();

	public:
		void bind() const override;
		void unbind() const override;
		void add_vertex_buffer(vertex_buffer& vbo);
		void add_index_buffer(index_buffer& ibo);

	private:
		std::uint32_t m_id{};
		std::vector<vertex_buffer> m_vertex_buffers;
		index_buffer m_index_buffer;
	};
}