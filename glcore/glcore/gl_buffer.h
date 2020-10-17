#pragma once

#include <cstdint>

namespace glcore
{
	class gl_buffer
	{
	protected:
		virtual ~gl_buffer() = default;
		virtual void bind() const = 0;
		virtual void unbind() const = 0;

	protected:
		std::uint32_t m_id{};
	};
}