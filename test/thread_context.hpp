#pragma once

#include <thread>
#include <future>

#include "thread_mgr.hpp"

#include <functional>

class thread_context
{
	using task = std::function<void(thread_context &)>;
public:
	thread_context(const thread_context &&c) : task_(std::move(c.task_)), 
		executed_(false), counter_(0)
	{
	}

	thread_context(task t) : task_(std::move(t)), executed_(false), counter_(0)
	{
	}

	template <typename type, typename ...task_types>
	void sync(task_types &&...tasks) {
		++counter_;

		thread_mgr::instance().sync<type>(std::move(tasks)...,
		[this]() {
			// we done!
			if ((counter_.fetch_sub(1) == 1) && executed_.load()) {

			}
		});
	}

	inline void operator()()
	{
		if (!executed_.load()) {
			task_(*this);
//			executed_ = true;
			executed_.store(true);
		}
		
		// we done!
		if (counter_.load() == 0) {
		}
	}

	inline bool need_switch()
	{
		return counter_.load() != 0;
	}

private:
	task task_;
//	bool executed_;
	std::atomic<bool> executed_;
	std::atomic<int> counter_;
};
