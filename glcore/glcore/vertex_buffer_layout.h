#pragma once

#include <vector>
#include <string_view>

namespace glcore
{
	struct sdt // Shader data type
	{
		enum class type { vec2, vec3, vec4 };
		static std::size_t size(type t_type);
		static std::uint32_t underlying_type(type t_type);
		static std::uint32_t component_count(type t_type);
	};
}

namespace glcore
{
	struct attribute_t
	{
		sdt::type type;
		std::string_view name;
	};

	struct vb_attribute
	{
		vb_attribute(attribute_t attribute);
		attribute_t data;
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
