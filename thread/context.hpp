#pragma once

#include "config.hpp"

#include <atomic>
#include <functional>
#include <type_traits>

THREAD_NS_BEGIN

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

	inline void operator()() 
	{
		task_();
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

	inline void operator()()
	{
		task_(context_);
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
