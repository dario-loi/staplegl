#pragma once

#include <cstdint>

namespace glcore
{
	class index_buffer
	{
	public:
		index_buffer() = default;
		index_buffer(std::uint32_t* indices, std::uint32_t count);
		~index_buffer();

	public:
		void bind() const;
		void unbind() const;
		std::uint32_t count() const;

	private:
		std::uint32_t m_id{};
		std::uint32_t m_count{};
	};
}