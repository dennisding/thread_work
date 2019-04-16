
#include "thread.hpp"

#include <iostream>

void do_something(thread::context &context)
{
	std::cout << "do something" << std::endl;
	context.sync<thread::any>([]() {
		std::cout << "do something in any" << std::endl;
	});
}

int main(int argc, const char **argv)
{
	thread::init();
	thread::setup();

	thread::sync<thread::any>(
	[]() {
		std::cout << "task 1" << std::endl;
	},
	thread::work_at<thread::render>([](thread::context &context) {
		std::cout << "task 2" << std::endl;

		context.sync<thread::render>([]() {
			std::cout << "work at render" << std::endl;
		});

		context.sync<thread::render>([]() {
			std::cout << "work at render 2" << std::endl;
		});

		context.sync<thread::render>([]() {
			std::cout << "work at render 3" << std::endl;
		});
	}),
	[](thread::context &context) {
		std::cout << "task 3" << std::endl;
		for (int i = 0; i < 10; ++i) {
			context.sync<thread::any>(
				[i]() {
					std::cout << i;
					std::this_thread::yield();
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