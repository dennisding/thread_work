#pragma once

#include "thread_executor.hpp"

#include <list>
#include <tuple>
#include <mutex>
#include <vector>
#include <memory>
#include <functional>

class thread_mgr;

class package_task : public std::enable_shared_from_this<package_task>
{
public:
	virtual ~package_task() {}
	virtual void operator()() {}
	virtual bool need_switch() { return false; }
	virtual void dispatch_self() {}
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

template <typename type, typename ...task_types>
class sync_task : public package_task
{
	//using task_tuple = std::tuple<task_types...>;
	using task_tuple = std::tuple<typename store_type<thread_context, task_types>::type...>;
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

	virtual void dispatch_self()
	{
		thread_mgr::instance().dispatch<type>(this);
	}

private:
	//task_selector selector_;
	executor executor_;
	task_tuple tasks_;
};

