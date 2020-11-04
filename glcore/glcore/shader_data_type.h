#pragma once

#include <cstddef>
#include <cstdint>

namespace glcore
{
	struct st // Shader data type
	{
		enum class type { vec2, vec3, vec4 };
		static std::size_t size(type t_type);
		static std::uint32_t underlying_type(type t_type);
		static std::uint32_t component_count(type t_type);
	};
}