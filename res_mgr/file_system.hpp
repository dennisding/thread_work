#pragma once

#include "binary.hpp"

#include <string>

class file_system
{
public:
	virtual ~file_system() {}

	virtual binary_ptr read(const std::string &name) = 0;

};

using file_system_ptr = std::shared_ptr<file_system>;

#include "win_file_system_imp.hpp"

using local_file_system = win_file_system;