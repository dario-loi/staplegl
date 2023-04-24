#pragma once

#include "vertex_buffer.h"
#include "index_buffer.h"

namespace glcore
{
	class vertex_array
	{
	public:
		vertex_array();
		~vertex_array();

	public:
		void bind() const;
		void unbind() const;
		void add_vertex_buffer(const vertex_buffer& vbo);
		void set_index_buffer(const index_buffer& ibo);

	private:
		std::uint32_t m_id{};
		std::vector<vertex_buffer> m_vertex_buffers;
		index_buffer m_index_buffer;
	};
}