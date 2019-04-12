#pragma once

#include "config.hpp"

#include <atomic>
#include <functional>
#include <type_traits>

THREAD_NS_BEGIN

// forward definition
template <typename type, typename ...task_types>
void sync_imp(task_types &...tasks);

class context
{
public:
	context() : counter_(0), executed_(false)
	{
	}

	template <typename type, typename ...task_types>
	void sync(task_types &...tasks)
	{
		++counter_;
		sync_imp<type>(tasks..., [this]() { --counter_; });

		executed_.store(true);
	}

private:
	std::atomic<int> counter_;
	std::atomic<bool> executed_;
};

template <typename task_type, int has_context>
class context_imp
{
public:
	context_imp(task_type &task) : task_(std::move(task))
	{

	}

	inline void operator()() 
	{
		task_();
	}

private:
	task_type task_;
};

template <typename task_type>
class context_imp<task_type, 1>
{
public:
	context_imp(task_type &task) : task_(std::move(task))
	{
	}

	inline void operator()()
	{
		task_(context_);
	}

private:
	task_type task_;
	context context_;
};

template <typename raw_type>
struct select_context
{
	using context_type = std::function<void(context &)>;
	using type = context_imp<raw_type, std::is_convertible<raw_type, context_type>::value>;
};

THREAD_NS_END
