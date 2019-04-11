#pragma once

#include "thread.hpp"

#include <tuple>

class package_task;
class thread_context;

template <typename type>
inline bool need_switch(const type &v)
{
	return false;
}

bool need_switch(thread_context &task);

//template <typename last, typename current>
//struct get_working_type
//{
//	using type = last;
//};
//
//template <typename thread_mgr,
//	typename tuple, 
//	size_t size, 
//	size_t index, 
//	typename last, 
//	typename current>
//struct executor_selector
//{
//	static bool executor(tuple &t, void **in, package_task *task)
//	{
//		*in = executor_selector<thread_mgr, tuple, size, index, current, current>::executor;
//		// dispatch to other thread
//		thread_mgr::instance().dispatch<current>(task);
//		return false;
//	}
//};
//
//template <typename thread_mgr, typename tuple, size_t size, typename last, typename current>
//struct executor_selector<thread_mgr, tuple, size, size, last, current>
//{
//	static bool executor(tuple &t, void **in, package_task *task)
//	{
//		*in = nullptr;
//		return false;
//	}
//};
//
//template <typename thread_mgr, typename tuple, size_t size, typename current>
//struct executor_selector<thread_mgr, tuple, size, size, current, current>
//{
//	static bool executor(tuple &t, void **in, package_task *task)
//	{
//		*in = nullptr;
//		return false;
//	}
//};
//
//template <typename thread_mgr, typename tuple, size_t size, size_t index, typename current>
//struct executor_selector<thread_mgr, tuple, size, index, current, current>
//{
//	static bool executor(tuple &t, void **in, package_task *task) {
////		using next_type = typename get_working_type<type, decltype(std::get<index>(t))>::type;
//		using next_type = decltype(thread::get_working_thread<current>(std::get<index>(t)));
//		*in = executor_selector<thread_mgr, tuple, size, index + 1, current, next_type>::executor;
//
//		std::get<index>(t)();
//
//		return !need_switch(std::get<index>(t));
//	}
//};

template <typename thread_mgr, 
	typename working,
	typename tuple, 
	size_t size, 
	size_t index, 
	typename last,
	typename current>
struct  executor_selector
{
	static bool executor(tuple &t, void **in, package_task *task)
	{
		// execute current task in different thread
//		using next_type = decltype(thread::get_working_thread<working>(std::get<size - index + 1>(t)));
		*in = executor_selector<thread_mgr, working, tuple, size, index, current, current>::executor;

		// need switch
		thread_mgr::instance().dispatch<current>(task);
		return false;
	}
};

template <typename thread_mgr, typename working, typename tuple, size_t size, typename last, typename current>
struct executor_selector<thread_mgr, working, tuple, size, 1, last, current>
{
	static bool executor(tuple &t, void **in, package_task *task)
	{
		// last task in different working thread
		*in = executor_selector<thread_mgr, working, tuple, size, 1, current, current>::executor;
		thread_mgr::instance().dispatch<current>(task);
		return false;
	}
};

template <typename thread_mgr, typename working, typename tuple, size_t size, typename current>
struct executor_selector<thread_mgr, working, tuple, size, 1, current, current>
{
	static bool executor(tuple &t, void **in, package_task *task)
	{
		// last task in same working thread
		*in = nullptr;
		std::get<size - 1>(t)();
		return false;
	}
};

template <typename thread_mgr, typename working,  typename tuple, size_t size, size_t index, typename current>
struct executor_selector<thread_mgr, working, tuple, size, index, current, current>
{
	static bool executor(tuple &t, void **in, package_task *task)
	{
		// in the same working thread
		using real_current = decltype(thread::get_working_thread<working>(std::get<size - index>(t)));
		using next_type = decltype(thread::get_working_thread<working>(std::get<size - index + 1>(t)));

		*in = executor_selector<thread_mgr, working, tuple, size, index - 1, real_current, next_type>::executor;

		std::get<size - index>(t)();
		return true;
	}
};
