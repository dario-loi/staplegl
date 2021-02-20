#pragma once

#include <vector>
#include <string_view>

#include "shader_data_type.h"

namespace glcore
{
	struct vb_attribute
	{
		vb_attribute(shader_data_type::type type, std::string_view name);
		shader_data_type::type type;
		std::string_view name;
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
		std::vector<vb_attribute>::const_iterator cbegin();
		std::vector<vb_attribute>::const_iterator cend();

	public:
		vb_attribute& operator[](std::size_t index);
		const vb_attribute& operator[](std::size_t index) const;
 
	private:
		std::uint32_t m_stride{};
		std::vector<vb_attribute> m_attributes;
	};
}
