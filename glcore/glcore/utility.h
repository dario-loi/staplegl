#pragma once

#include <string>
#include <fstream>

namespace glcore::util
{
	static bool is_path(const char* path)
	{
		std::ifstream file(path);
		return (file) ? true : false;
	}

	static std::string read_file(const char* path)
	{
		std::string result;
		std::ifstream in(path, std::ios::in);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		return result;
	}
}