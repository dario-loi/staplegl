#pragma once

#include <vector>
#include <string_view>

#include "shader_data_type.h"

namespace glcore
{
	struct vb_attribute_t
	{
		st::type type;
		std::string_view name;
	};

	struct vb_attribute
	{
		vb_attribute(vb_attribute_t attribute);
		vb_attribute_t data;
		std::uint32_t offset{};
	};

	class vertex_buffer_layout
	{
	public:
		vertex_buffer_layout() = default;
		vertex_buffer_layout(std::initializer_list<vb_attribute> attributes);

	public:
		std::uint32_t stride() const;
		std::vector<vb_attribute>& data();
		std::vector<vb_attribute>::iterator begin();
		std::vector<vb_attribute>::iterator end();

	public:
		vb_attribute& operator[](std::size_t index);
		const vb_attribute& operator[](std::size_t index) const;
 
	private:
		std::uint32_t m_stride{};
		std::vector<vb_attribute> m_attributes;
	};
}
