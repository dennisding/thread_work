
#include "thread_mgr.hpp"

#include <iostream>


int main(int argc, const char **argv)
{
	thread_mgr::instance().init();

	task_list<thread::main>::add_task([]() {
		std::cout << "task in main" << std::endl;
	});

	task_list<thread::main>::add_task([]() {
		std::cout << "task in main2222" << std::endl;
	});

	task_list<thread::render>::add_task([]() {
		std::cout << "task in render" << std::endl;
	});

	thread_mgr::instance().tick<thread::main>();

	thread_mgr::instance().sync<thread::main>(
		[]() {
		std::cout << "task 1" << std::endl;
		},
		[]() {
			std::cout << "task 2" << std::endl;
		},
		thread::work_at<thread::render>([]() {
			std::cout << "task 3" << std::endl;
		}),
		[]() {
			std::cout << "task 4" << std::endl;
		}
	);
}