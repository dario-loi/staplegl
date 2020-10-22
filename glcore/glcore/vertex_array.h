#pragma once

#include "gl_object.h"
#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"

namespace glcore
{
	class vertex_array : public gl_object
	{
	public:
		vertex_array();
		~vertex_array();

	public:
		void add_vertex_buffer(vertex_buffer& vbo, vertex_buffer_layout& layout);
		void bind() const;
		void unbind() const;
	};
}