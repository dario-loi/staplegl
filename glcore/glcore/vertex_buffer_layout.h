#pragma once

#include <vector>

namespace glcore
{
	struct element_t
	{
		std::uint32_t count{};
		std::uint32_t type{};
		bool normalized{};
	};

	class vertex_buffer_layout
	{
	public:
		template<typename T>
		void push_element(std::uint32_t count);
		template<> void push_element<float>(std::uint32_t count);
		template<> void push_element<std::uint32_t>(std::uint32_t count);

	public:
		std::vector<element_t>& elements();
		std::uint32_t stride() const;

	public:
		element_t& operator[](std::size_t index);
		const element_t& operator[](std::size_t index) const;
 
	private:
		std::uint32_t m_stride{};
		std::vector<element_t> m_elements;
	};
}

#include "vertex_buffer_layout_impl.h"