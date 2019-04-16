#pragma once

#include "config.hpp"
#include "context.hpp"
#include "executor.hpp"
#include "thread_task.hpp"

#include <list>
#include <tuple>
#include <mutex>
#include <thread>
#include <memory>

THREAD_NS_BEGIN

template <typename type, typename ...task_types>
class package_task : public task
{
	using task_tuple = std::tuple<typename select_context<typename get_working_type<type, task_types>::type, task_types>::type...>;
	using executor_type = decltype(get_executor<task_tuple, task, type>());
public:
	package_task(task_types &...tasks) : tasks_(tasks...)
	{
		executor_ = get_executor<task_tuple, task, type>();
	}

	package_task(task_types &&...tasks) : tasks_(std::forward<task_types>(tasks)...)
	{
		executor_ = get_executor<task_tuple, task, type>();
	}

	virtual void operator()()
	{
		// execute task
		while (!(executor_->execute(tasks_, executor_, this))) {
		}
	}

	virtual void dispatch_self()
	{
		dispatch<type>(this->shared_from_this());
	}

private:
	executor_type executor_;
	task_tuple tasks_;
};

template <typename ...type>
class task_list
{
};

template <typename type>
class task_list<type>
{
	using task_list_type = std::list<task_ptr>;

public:
	inline static task_list &instance()
	{
		static task_list ins_;
		return ins_;
	}

	void execute()
	{
		task_ptr task;
		for (;;) {
			// get the task
			{
				std::lock_guard<std::mutex> lock(mutex_);
				if (tasks_.empty()) {
					break;
				}
				else {
					task = *tasks_.begin();
					tasks_.pop_front();
				}
			}
			// execute the task
			(*task)();
		}
	}

	template <typename ...task_types>
	void add_task(task_types &&...tasks)
	{
		using task_type = package_task<type, typename std::remove_reference<task_types>::type...>;

		dispatch(std::make_shared<task_type>(std::forward<task_types>(tasks)...));
	}

	inline void dispatch(task_ptr &&task)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		tasks_.push_back(std::forward<task_ptr>(task));
	}

private:
	std::mutex mutex_;
	task_list_type tasks_;
};

template <typename head, typename ...tails>
class task_list<head, tails...>
{
public:
	inline static task_list &instance()
	{
		static task_list ins_;
		return ins_;
	}

	void execute()
	{
		task_list<head>::instance().execute();
		task_list<tails...>::instance().execute();
	}
};

THREAD_NS_END

