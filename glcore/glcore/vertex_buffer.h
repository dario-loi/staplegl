#pragma once

#include "gl_object.h"

namespace glcore
{
	class vertex_buffer : public gl_object
	{
	public:
		vertex_buffer() = default;
		vertex_buffer(float* vertices, std::uint32_t size);
		~vertex_buffer();

	public:
		void bind() const override;
		void unbind() const override;
	};
}