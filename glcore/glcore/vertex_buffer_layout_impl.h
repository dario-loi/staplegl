#include "vertex_buffer_layout.h"

namespace glcore
{
	template<typename T>
	void vertex_buffer_layout::push_element(std::uint32_t count)
	{}

	template<>
	void vertex_buffer_layout::push_element<float>(std::uint32_t count)
	{
		m_elements.push_back({ count, 0x1406 , false});
		m_stride += count * sizeof(float);
	}

	template<>
	void vertex_buffer_layout::push_element<std::uint32_t>(std::uint32_t count)
	{
		m_elements.push_back({ count, 0x1405 , false });
		m_stride += count * sizeof(std::uint32_t);
	}

	inline std::uint32_t glcore::vertex_buffer_layout::stride() const
	{
		return m_stride;
	}

	inline element_t& vertex_buffer_layout::operator[](std::size_t index)
	{
		return m_elements[index];
	}

	inline const element_t& vertex_buffer_layout::operator[](std::size_t index) const
	{
		return m_elements[index];
	}
}