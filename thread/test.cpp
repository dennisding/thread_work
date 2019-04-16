
#include "thread.hpp"

#include <iostream>

void do_something(thread::context &context)
{
	std::cout << "do something" << std::endl;
	context.sync<thread::any>([]() {
		std::cout << "do something in any" << std::endl;
	});
}

int g_count;

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
		using counter_type = std::atomic<int>;

		auto counter = std::make_shared<counter_type>(0);

		context.on_done([=]() {
			std::cout << "on done!!!" << counter->load() << std::endl;
		});

		for (int i = 0; i < 102400; ++i) {
			context.sync<thread::any>(
				[=](thread::context &context) {
					(*counter)++;
//					++g_count;
//					std::cout << i;
//					std::this_thread::yield();
				}
			);
		}
	},
	thread::work_at<thread::io>(&do_something)
	);

	while (true) {
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
}