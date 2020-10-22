#pragma once

#include "gl_object.h"

namespace glcore
{
	class index_buffer : public gl_object
	{
	public:
		index_buffer() = default;
		index_buffer(std::uint32_t* indices, std::uint32_t count);
		~index_buffer();

	public:
		void bind() const override;
		void unbind() const override;
		std::uint32_t count() const;

	private:
		std::uint32_t m_count;
	};
}