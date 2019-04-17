#pragma once

#include "config.hpp"

#include <atomic>
#include <iostream>
#include <functional>
#include <type_traits>

THREAD_NS_BEGIN

class context
{
	using callback_type = std::function<void()>;
public:
	inline context() : has_sub_task_(false), counter_(2), on_done_(nullptr)
	{
	}

	template <typename type, typename ...task_types>
	void sync(task_types &&...tasks)
	{
//		++counter_;
		counter_.fetch_add(1, std::memory_order_release);
		has_sub_task_ = true;

		sync_imp<type>(std::forward<task_types>(tasks)...,
			[this]() {
				finish_one_task(1);
		});
	}

	inline void executed()
	{
		finish_one_task(2);
	}

	void set_task(task *task)
	{
		task_ = task;
	}

	inline bool has_sub_task()
	{
		return has_sub_task_;
	}

	inline void on_done(callback_type &&callback)
	{
		on_done_ = callback;
	}

private:
	inline void finish_one_task(int count)
	{
		if (counter_.fetch_sub(count, std::memory_order_acq_rel) == 1) {
			// all work done , dispatch to working thread
			if (on_done_) {
				on_done_();
				on_done_ = nullptr;
			}
			task_->dispatch_self();
		}
	}

private:
	task *task_;
	bool has_sub_task_;
	std::atomic<int> counter_;
	callback_type on_done_;
};

template <typename working, typename task_type, int has_context>
class context_imp
{
public:
	using type = working;

	context_imp(task_type &task) : task_(task)
	{
	}

	context_imp(task_type &&task) : task_(std::move(task))
	{
	}

	// is the work is done, return true
	// else return false
	inline bool operator()(task *t) 
	{
		task_();
		return true;
	}

private:
	task_type task_;
};

template <typename working, typename task_type>
class context_imp<working, task_type, 1>
{
public:
	using type = working;

	context_imp(task_type &task) : task_(task)
	{
	}

	context_imp(task_type &&task) : task_(std::move(task))
	{
	}

	inline bool operator()(task *t)
	{
		context_.set_task(t);

		task_(context_);

		context_.executed();

		return !context_.has_sub_task();
	}

private:
	task_type task_;
	context context_;
};

template <typename type>
struct has_context : public std::integral_constant<int,
	std::is_convertible<type, std::function<void(context &)>>::value>
{

};

template <typename working, typename raw_type>
struct select_context
{
	using type = typename context_imp<working, raw_type, has_context<raw_type>::value>;
};

THREAD_NS_END
