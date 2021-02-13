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

	static std::string get_file_name(const char* path)
	{
		std::string filepath = path;
		auto last_slash = filepath.find_last_of("/\\");
		last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;
		auto last_dot = filepath.rfind('.');
		auto count = last_dot == std::string::npos ? filepath.size() - last_slash : last_dot - last_slash;
		return filepath.substr(last_slash, count);
	}
}