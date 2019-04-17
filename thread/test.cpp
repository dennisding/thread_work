
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

	thread::sync<thread::io>([](thread::context &context) {
		std::cout << "task 1" << std::endl;
		auto counter = std::make_shared<std::atomic<int>>();

		context.on_done([counter]() {
			std::cout << std::endl;
			std::cout << "on done!!!" << counter->load(std::memory_order_acquire) << std::endl;
		});

 		for (int i = 0; i < 10; ++i) {
			context.sync<thread::any>([i, counter]() {
//				counter->fetch_add(1, std::memory_order_release);
				counter->fetch_add(1, std::memory_order_release);
				std::cout << i;
			});
		}
	},
	thread::work_at<thread::script>([](thread::context &context) {
		std::cout << "task 2" << std::endl;
		for (int i = 1; i < 8; ++i) {
			context.sync<thread::render>([i]() {
				std::cout << i;
			});
		}
	})
	);

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}