#pragma once

#include "config.hpp"
#include "context.hpp"
#include "executor.hpp"

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
		dispatch<type>(this);
	}

private:
	executor_type executor_;
	task_tuple tasks_;
};

template <typename ...type>
class task_list
{
};

class task_list_imp
{
public:
	inline task_list_imp() : head_(nullptr)
	{
		tail_ = &head_;
	}

	inline void push_back(task *t)
	{
		(*tail_) = t;
		tail_ = &t->next_;
	}

	inline task *pop_front()
	{
		if (head_ == nullptr) {
			return nullptr;
		}

		task *result = head_;
		head_ = head_->next_;
		if (!head_) {
			tail_ = &head_;
		}

		return result;
	}

private:
	task *head_;
	task **tail_;
};

template <typename type>
class task_list<type>
{
	using task_list_type = std::list<task_ptr>;

public:
	task_list() : tasks_(new task_list_imp)
	{
	}

	inline static task_list &instance()
	{
		static task_list ins_;
		return ins_;
	}

	void execute()
	{
		for (;;) {
			task *work = pop_front();
			if (work) {
				(*work)();
			}
			else {
				break;
			}
		}
	}

	template <typename ...task_types>
	inline void add_task(task_types &&...tasks)
	{
		using task_type = package_task<type, typename std::remove_reference<task_types>::type...>;

		push_back(new task_type(std::forward<task_types>(tasks)...));
	}

	inline void dispatch(task *t)
	{
		push_back(t);
	}

private:
	inline void push_back(task *t)
	{
		// get the list implement
		task_list_imp *list = nullptr;
		do {
			list = tasks_.exchange(nullptr, std::memory_order_acquire);
		} while (!list);

		list->push_back(t);

		tasks_.exchange(list, std::memory_order_release);
	}

	inline task *pop_front()
	{
		// get the list implement
		task_list_imp *list = nullptr;
		do {
			list = tasks_.exchange(nullptr, std::memory_order_acquire);
		} while (!list);

		auto t = list->pop_front();

		tasks_.exchange(list, std::memory_order_release);

		return t;
	}
private:
	std::atomic<task_list_imp *> tasks_;
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

