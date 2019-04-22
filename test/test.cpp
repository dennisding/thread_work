
#include <iostream>

#include <thread/thread.hpp>

#include <res_mgr/res_mgr.hpp>

int main(int argc, const char **argv)
{
	res_mgr::instance().setup();

	auto bin = res_mgr::instance().read<binary>("test.txt");

	auto info = res_mgr::instance().read<res>("test.txt");

	auto result = info->read<std::string>("block");
}