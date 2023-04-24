#pragma once

#include <vector>
#include <string_view>

#include "shader_data_type.h"

namespace glcore
{
	struct vertex_attribute
	{
		shader_data_type::type_t type;
		std::string_view name;
		std::uint32_t offset{};
	};

	class vertex_buffer_layout
	{
	public:
		vertex_buffer_layout() = default;
		vertex_buffer_layout(std::initializer_list<vertex_attribute> attributes);

	public:
		std::uint32_t stride() const;
		const std::vector<vertex_attribute>& data() const;
 
	private:
		std::uint32_t m_stride{};
		std::vector<vertex_attribute> m_attributes;
	};
}
