
#include "thread_mgr.hpp"
#include "thread_context.hpp"

#include <iostream>


int main(int argc, const char **argv)
{
	thread_mgr::instance().init();

	thread_mgr::instance().sync<thread::logic>(
	[]() {
		std::cout << "task 1" << std::endl;
	},
	thread::work_at<thread::any>([](){
		std::cout << "task 2" << std::endl;
	}),
	[]() {
		std::cout << "task 3" << std::endl;
	},
	thread::work_at<thread::render>([]() {
		std::cout << "task 4" << std::endl;
	})
	);

	while (true) {
		std::this_thread::yield();
	}

	thread_mgr::instance().quit(true);

	thread_mgr::instance().join();
}