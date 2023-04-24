#pragma once

#include "vertex_buffer_layout.h"

namespace glcore
{
	class vertex_buffer
	{
	public:
		vertex_buffer() = default;
		vertex_buffer(float* vertices, std::uint32_t size);
		vertex_buffer(float* vertices, std::uint32_t size, const vertex_buffer_layout& layout);
		~vertex_buffer();

	public:
		void bind() const;
		void unbind() const;
		void set_layout(const vertex_buffer_layout& layout);
		const vertex_buffer_layout& layout() const;

	private:
		std::uint32_t m_id{};
		vertex_buffer_layout m_layout;
	};
}