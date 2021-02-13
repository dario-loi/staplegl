#include "shader_data_type.h"

namespace glcore
{
	std::size_t shader_data_type::size(type t_type)
	{
		switch (t_type)
		{
		case type::vec2: return sizeof(float) * 2; break;
		case type::vec3: return sizeof(float) * 3; break;
		case type::vec4: return sizeof(float) * 4; break;
		}
	}

	std::uint32_t shader_data_type::underlying_type(type t_type)
	{
		switch (t_type)
		{
		case type::vec2: return 0x1406; break;
		case type::vec3: return 0x1406; break;
		case type::vec4: return 0x1406; break;
		}
	}

	std::uint32_t shader_data_type::component_count(type t_type)
	{
		switch (t_type)
		{
		case type::vec2: return 2; break;
		case type::vec3: return 3; break;
		case type::vec4: return 4; break;
		}
	}
}