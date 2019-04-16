
#include "thread.hpp"

#include <iostream>

void do_something()
{
	std::cout << "do something" << std::endl;
}

int main(int argc, const char **argv)
{
	thread::init();
	thread::setup();

	thread::sync<thread::any>(
	[]() {
		std::cout << "task 1" << std::endl;
	},
	thread::work_at<thread::render>([]() {
		std::cout << "task 2" << std::endl;
	}),
	[](thread::context &context) {
		std::cout << "task 3" << std::endl;
		for (int i = 0; i < 10; ++i) {
			context.sync<thread::any>(
				[i]() {
					std::cout << i;
				}
			);
		}
	},
	thread::work_at<thread::io>(&do_something)
	);

	while (true) {
		std::this_thread::yield();
	}
}