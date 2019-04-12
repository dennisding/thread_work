#pragma once

#include <functional>

template <typename thread_context, typename raw_type>
struct store_type
{
	// we have 2 special function signature
	// void () {}
	// void (thread_context &c) {}
	template <int n>
	struct store_type_imp
	{
		using type = raw_type;
	};

	template <>
	struct store_type_imp<1>
	{
		using type = thread_context;
	};

	using type = typename store_type_imp<std::is_convertible<raw_type, std::function<void(thread_context&)>>::value>::type;
};

template <typename thread_context, typename return_type>
struct store_type<thread_context, return_type(*)(thread_context &c)>
{
	using type = thread_context;
};

namespace thread
{
	struct main {};
	struct io {};
	struct render {};
	struct logic {};
	struct any {};

	template <typename type, typename task_type>
	struct work_at_imp
	{
		using store = typename store_type<task_type>::type;
		work_at_imp(task_type task) : task_(std::move(task))
		{
		}

		work_at_imp(work_at_imp &w) : task_(std::move(w.task_))
		{
		}

		work_at_imp(work_at_imp &&w) : task_(std::move(w.task_))
		{
		}


		inline void operator()() { task_(); }

		store task_;
	};

	template <typename type, typename task_type>
	work_at_imp<type, task_type> work_at(task_type &&task)
	{
		return work_at_imp<type, task_type>(std::move(task));
	}

	template <typename working, typename type>
	working get_working_thread(type t)
	{
		return working();
	}

	template <typename working, typename working_type, typename task>
	working_type get_working_thread(work_at_imp<working_type, task> t)
	{

	}
};