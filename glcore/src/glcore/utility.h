#pragma once

#include <string>
#include <fstream>

namespace glcore::util
{
	static bool is_path(std::string_view path)
	{
		std::ifstream file(path.data());
		return (file) ? true : false;
	}

	static std::string_view read_file(std::string_view path)
	{
		std::string result;
		std::ifstream in(path.data(), std::ios::in);

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

	static std::string get_file_name(std::string_view path)
	{
		std::string filepath = path.data();
		auto last_slash = filepath.find_last_of("/\\");
		last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;
		const auto last_dot = filepath.rfind('.');
		const auto count = last_dot == std::string::npos ? filepath.size() - last_slash : last_dot - last_slash;
		return filepath.substr(last_slash, count);
	}
}