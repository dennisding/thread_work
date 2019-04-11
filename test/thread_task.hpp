#pragma once

#include "thread_executor.hpp"

#include <list>
#include <tuple>
#include <mutex>
#include <vector>
#include <memory>
#include <functional>


//using thread_task = std::function<void()>;
//
//template <typename ...types>
//struct task_list
//{
//
//};
//
//template <typename head>
//struct task_list<head>
//{
//	using list = std::list<std::function<void()>>;
//
//	static thread_task get_task()
//	{
//		auto &tasks = get_task_list();
//		if (tasks.empty()) {
//			return nullptr;
//		}
//
//		auto task = std::move(*(tasks.begin()));
//		tasks.pop_front();
//		return task;
//	}
//
//	static void add_task(thread_task &&task)
//	{
//		get_task_list().emplace_back(task);
//	}
//
//	static list &get_task_list()
//	{
//		static list ins_;
//		return ins_;
//	}
//};
//
//template <typename head, typename ...tails>
//struct task_list<head, tails...>
//{
//	static thread_task get_task()
//	{
//		auto task = task_list<head>::get_task();
//		if (task) {
//			return std::move(task);
//		}
//
//		return task_list<tails...>::get_task();
//	}
//};

class package_task : public std::enable_shared_from_this<package_task>
{
public:
	virtual ~package_task() {}
	virtual void operator()() {}
	virtual bool need_switch() { return false; }
};

using package_task_ptr = std::shared_ptr<package_task>;

template <typename type>
class task_list
{
	using tasks = std::list<package_task_ptr>;
public:
	package_task_ptr get_task()
	{
		std::lock_guard<std::mutex> lock(mutex_);

		if (tasks_.empty()) {
			return nullptr;
		}

		auto task = *tasks_.begin();
		tasks_.pop_front();
		return task;
	}

	static task_list<type> &instance()
	{
		static task_list<type> ins_;
		return ins_;
	}

	void add_task(const package_task_ptr &task)
	{
		std::lock_guard<std::mutex> lock(mutex_);

		tasks_.push_back(task);
	}

private:
	tasks tasks_;
	std::mutex mutex_;
};

//using package_task_ptr = std::shared_ptr<package_task>;
//
//template <size_t N, size_t index, typename task_vector, typename last, typename current>
//struct task_selector_imp
//{
//	static bool selector(task_vector &v, void *&selector)
//	{
//		using next = decltype(std::get<N - index + 1>(v));
//		selector = task_selector_imp<N, index - 1, task_vector, current, next>::selector;
//		std::get<N - index>(v)();
//
//		return true;
//	}
//};
//
//template <size_t N, size_t index, typename task_vector, typename type>
//struct task_selector_imp<N, index, task_vector, type, type>
//{
//	static bool selector(task_vector &v, void *&selector)
//	{
//
//	}
//};
//
//template <size_t N, typename task_vector, typename last, typename current>
//struct task_selector_imp<N, 1, task_vector, last, current>
//{
//	static bool selector(task_vector &v, void *&selector)
//	{
//		selector = nullptr;
//		std::get<N - 1>(v)();
//		return false;
//	}
//};

class thread_mgr;

template <typename type, typename ...task_types>
class sync_task : public package_task
{
	using task_tuple = std::tuple<task_types...>;
	using executor = bool(*)(task_tuple &, void **, package_task *);
public:
	sync_task(task_types &&...tasks) : tasks_(std::move(tasks)...)
	{
		executor_ = executor_selector<thread_mgr, type, task_tuple, sizeof...(task_types), sizeof...(task_types), type, type>::executor;
	}

	virtual void operator()() 
	{
		bool has_next = true;

		while (has_next && executor_) {
			has_next = executor_(tasks_, (void **)&executor_, this);
		}
	}

private:
	//task_selector selector_;
	executor executor_;
	task_tuple tasks_;
};