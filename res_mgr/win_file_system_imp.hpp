#pragma once

#include "res.hpp"

class win_file_system : public file_system
{
public:
	virtual binary_ptr read(const std::string &name);

	static res_ptr load_config(const std::string& name);

private:
	static binary_ptr read_file(const std::string& name);
};
