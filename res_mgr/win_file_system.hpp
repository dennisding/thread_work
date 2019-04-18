#pragma once

#include "file_system.hpp"

class win_file_system : public file_system
{
public:
	binary_ptr read(const std::string &name);
};
