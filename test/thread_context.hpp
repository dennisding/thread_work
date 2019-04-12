#pragma once

#include <thread>
#include <future>

#include "thread_mgr.hpp"

#include <functional>

class thread_context
{
	using task = std::function<void(thread_context &)>;
public:
	thread_context(thread_context &c) : task_(std::move(c.task_)), package_task_(nullptr),
		executed_(false), counter_(0)
	{
	}

	thread_context(task t) : task_(std::move(t)), package_task_(nullptr), executed_(false), counter_(0)
	{
	}

	template <typename type, typename ...task_types>
	void sync(task_types &&...tasks) {
		++counter_;

		thread_mgr::instance().sync<type>(std::move(tasks)...,
		[this]() {
			// we done!
			if ((counter_.fetch_sub(1) == 1) && executed_.load()) {
				done();
			}
		});
	}

	inline void operator()()
	{
		if (!executed_.load()) {
			task_(*this);
			executed_.store(true);
		}
		
		// we done!
		if (counter_.load() == 0) {
			done();
		}
	}

	inline bool need_switch()
	{
		return counter_.load() != 0;
	}

	inline void done()
	{
//		thread_mgr::instance().dispatch<thread::any>(&*package_task_);
		package_task_->dispatch_self();
		package_task_ = nullptr;
	}

	void set_package_task(package_task *task)
	{
		package_task_ = task->shared_from_this();
	}

private:
	task task_;
	package_task_ptr package_task_;
	std::atomic<bool> executed_;
	std::atomic<int> counter_;
};
