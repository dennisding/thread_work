
#include "client.hpp"

#include <thread/thread.hpp>

client::client() : win_(nullptr), quit_(false)
{

}

void client::init(const win_ptr &win)
{
	win_ = win;
}

void client::tick()
{
	std::promise<bool> wait;
	auto future = wait.get_future();

	thread::sync<thread::main>(
		[this](thread::context &context) {
			// process_input
			win_->process_event();
		},
		thread::work_at<thread::render>([this]() {
			win_->swap_buffers();
		}),
		[this, &wait]() {
			// process render
			wait.set_value(true);
		}
	);

	for (;;) {
		auto state = future.wait_for(std::chrono::seconds(0));
		if (state == std::future_status::ready) {
			// frame done
			break;
		}
		// process main thread
		thread::tick<thread::main, thread::render>();
	}
}