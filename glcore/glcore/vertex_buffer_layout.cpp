#include "vertex_buffer_layout.h"

namespace glcore
{
	vertex_buffer_layout::vertex_buffer_layout(std::initializer_list<vb_attribute> attributes)
		: m_attributes{ attributes }
	{
		for (auto& [data, offset] : m_attributes)
		{
			offset = m_stride;
			m_stride += sdt::size(data.type);
		}
	}

	std::uint32_t vertex_buffer_layout::stride() const
	{
		return m_stride;
	}

	std::vector<vb_attribute>& vertex_buffer_layout::data()
	{
		return m_attributes;
	}

	std::vector<vb_attribute>::iterator vertex_buffer_layout::begin()
	{
		return m_attributes.begin();
	}

	std::vector<vb_attribute>::iterator vertex_buffer_layout::end()
	{
		return m_attributes.end();
	}

	vb_attribute& vertex_buffer_layout::operator[](std::size_t index)
	{
		return m_attributes[index];
	}

	const vb_attribute& vertex_buffer_layout::operator[](std::size_t index) const
	{
		return m_attributes[index];
	}

	vb_attribute::vb_attribute(attribute_t attribute)
		: data{ attribute }
	{}
}

namespace glcore
{
	std::size_t sdt::size(type t_type)
	{
		switch (t_type)
		{
		case type::vec2: return sizeof(float) * 2; break;
		case type::vec3: return sizeof(float) * 3; break;
		case type::vec4: return sizeof(float) * 4; break;
		}
	}

	std::uint32_t sdt::underlying_type(type t_type)
	{
		switch (t_type)
		{
		case type::vec2: return 0x1406; break;
		case type::vec3: return 0x1406; break;
		case type::vec4: return 0x1406; break;
		}
	}

	std::uint32_t sdt::component_count(type t_type)
	{
		switch (t_type)
		{
		case type::vec2: return 2; break;
		case type::vec3: return 3; break;
		case type::vec4: return 4; break;
		}
	}
}