

#include "file_system.hpp"

#include <cstdio>

res_ptr win_file_system::load_config(const std::string &name)
{
	return res::parse(read_file(name));
}

binary_ptr win_file_system::read_file(const std::string& name)
{
	FILE* file = nullptr;
	fopen_s(&file, name.c_str(), "rb");
	if (!file) {
		return nullptr;
	}

	fseek(file, 0, SEEK_END);
	auto size = ftell(file);
	fseek(file, 0, SEEK_SET);

	binary_ptr bin = std::make_shared<binary>(size);
	fread(bin->get_data(), 1, size, file);

	fclose(file);

	return bin;
}

binary_ptr win_file_system::read(const std::string &name)
{
	return read_file(name);
}