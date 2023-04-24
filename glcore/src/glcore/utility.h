#pragma once

#include <string>
#include <fstream>

namespace glcore::util
{
	static std::string read_file(std::string_view path)
	{
		std::string result;

		if (std::ifstream in(path.data(), std::ios::in); in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}

		return result;
	}

	static std::string get_file_name(std::string_view path)
	{
		auto last_slash = path.find_last_of("/\\");
		last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;

		const auto last_dot = path.rfind('.');
		const auto count = last_dot == std::string::npos ? path.size() - last_slash : last_dot - last_slash;

		return path.substr(last_slash, count).data();
	}
}