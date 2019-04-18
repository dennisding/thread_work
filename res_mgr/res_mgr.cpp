
#include "res_mgr.hpp"
#include "win_file_system.hpp"

#include <thread/thread.hpp>

#include <algorithm>

void res_mgr::setup()
{
	// add win file system
	add_file_system(std::make_shared<win_file_system>(), 0);
}

void res_mgr::add_file_system(const file_system_ptr &system, size_t pos)
{
	auto insert_pos = file_systems_.end();
	if (pos <= file_systems_.size()) {
		insert_pos = file_systems_.begin() + pos;
	}

	file_systems_.insert(insert_pos,  system);
}

binary_ptr res_mgr::open(const std::string &name)
{
	for (auto system : file_systems_) {
		auto bin = system->read(name);
		if (bin) {
			return bin;
		}
	}

	return nullptr;
}

void res_mgr::open(const std::string &name, open_callback &&callback)
{
	thread::sync<thread::io>([this, name, callback]() {
		callback(open(name));
	});
}