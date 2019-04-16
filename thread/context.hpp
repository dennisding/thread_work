#pragma once

#include "config.hpp"
#include "thread_task.hpp"

#include <atomic>
#include <iostream>
#include <functional>
#include <type_traits>

THREAD_NS_BEGIN

class context
{
public:
	inline context() : has_sub_task_(false), counter_(1)
	{
	}

	template <typename type, typename ...task_types>
	void sync(task_types &&...tasks)
	{
		++counter_;
		has_sub_task_ = true;

		sync_imp<type>(std::forward<task_types>(tasks)...,
			[this]() {
				finish_one_task();
		});
	}

	inline void executed()
	{
		finish_one_task();
	}

	void set_task(task_ptr &&task)
	{
		task_ = task;
	}

	inline bool has_sub_task()
	{
		return has_sub_task_;
	}

private:
	inline void finish_one_task()
	{
		if (counter_.fetch_sub(1) == 1) {
			// all work done , dispatch to working thread
			std::cout << "all work done!!!!" << std::endl;
			task_->dispatch_self();
		}
	}

private:
	task_ptr task_;
	bool has_sub_task_;
	std::atomic<int> counter_;
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
		context_.set_task(t->shared_from_this());

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
