#pragma once

#include "gl_object.h"
#include "vertex_buffer.h"

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

	private:
		std::uint32_t m_id{};
	};
}