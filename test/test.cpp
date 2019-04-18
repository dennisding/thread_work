
#include <iostream>

#include <thread/thread.hpp>

#include <res_mgr/res_mgr.hpp>

int main(int argc, const char **argv)
{
	thread::setup();
	res_mgr::instance().setup();

	res_mgr::instance().open("test.txt", [](const binary_ptr &bin) {
		std::cout << "file io" << std::endl;
		thread::sync<thread::render>([bin]() {
			std::cout << "work at render" << std::endl;
		});
	});

	thread::sync<thread::any>([]() {
		std::cout << "task 1" << std::endl;
	},
	thread::work_at<thread::io>([](thread::context &context) {
		std::cout << "task 2" << std::endl;
		// do some io work
	}),
	[](thread::context &context) {
		std::cout << "task 3" << std::endl;
		return 1.0;
	});

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}