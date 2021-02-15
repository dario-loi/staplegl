#pragma once

#include <cstddef>
#include <cstdint>

namespace glcore::shader_data_type
{
	enum class type
	{ 
		vec2, 
		vec3, 
		vec4 
	};

	static std::size_t size(type t_type)
	{
		switch (t_type)
		{
		case type::vec2: return sizeof(float) * 2; break;
		case type::vec3: return sizeof(float) * 3; break;
		case type::vec4: return sizeof(float) * 4; break;
		}
	}

	static std::uint32_t underlying_type(type t_type)
	{
		switch (t_type)
		{
		case type::vec2: return 0x1406; break;
		case type::vec3: return 0x1406; break;
		case type::vec4: return 0x1406; break;
		}
	}

	static std::uint32_t component_count(type t_type)
	{
		switch (t_type)
		{
		case type::vec2: return 2; break;
		case type::vec3: return 3; break;
		case type::vec4: return 4; break;
		}
	}
}