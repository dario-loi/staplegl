#pragma once

#include "gl_object.h"
#include "vertex_buffer_layout.h"

namespace glcore
{
	class vertex_buffer : public gl_object
	{
	public:
		vertex_buffer() = default;
		vertex_buffer(float* vertices, std::uint32_t size);
		vertex_buffer(float* vertices, std::uint32_t size, vertex_buffer_layout layout);
		~vertex_buffer();

	public:
		void bind() const override;
		void unbind() const override;
		void set_layout(vertex_buffer_layout& layout);
		vertex_buffer_layout& layout();

	private:
		std::uint32_t m_id{};
		vertex_buffer_layout m_layout;
	};
}