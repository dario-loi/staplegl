#pragma once

#include <cstdint>

namespace glcore
{
	class gl_object
	{
	protected:
		virtual ~gl_object() = default;
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
	};
}