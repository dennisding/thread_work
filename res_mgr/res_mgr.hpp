#pragma once

#include "res.hpp"
#include "file_system.hpp"

#include <utils/singleton.hpp>

#include <vector>
#include <limits>
#include <functional>

class res_mgr : public singleton<res_mgr>
{
	using file_system_vector = std::vector<file_system_ptr>;
	using open_callback = std::function<void(binary_ptr &&bin)>;
public:
	void setup();

	binary_ptr read(const std::string &name);
	void read(const std::string &name, open_callback &&callback);

	template <typename type>
	std::shared_ptr<type> read(const std::string& name)
	{
		return res_info<type>::parse(read(name));
	}

	template <typename type>
	void read(const std::string& name, std::function<void(const std::shared_ptr<type> &)>&& callback)
	{
		callback(read<type>(name));
	}

	void add_file_system(const file_system_ptr &system, size_t pos = std::numeric_limits<int>::max());

private:
	file_system_vector file_systems_;
};
