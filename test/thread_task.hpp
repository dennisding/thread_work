#pragma once

#include <list>
#include <tuple>
#include <vector>
#include <memory>
#include <functional>

using thread_task = std::function<void()>;

template <typename ...types>
struct task_list
{

};

template <typename head>
struct task_list<head>
{
	using list = std::list<std::function<void()>>;

	static thread_task get_task()
	{
		auto &tasks = get_task_list();
		if (tasks.empty()) {
			return nullptr;
		}

		auto task = std::move(*(tasks.begin()));
		tasks.pop_front();
		return task;
	}

	static void add_task(thread_task &&task)
	{
		get_task_list().emplace_back(task);
	}

	static list &get_task_list()
	{
		static list ins_;
		return ins_;
	}
};

template <typename head, typename ...tails>
struct task_list<head, tails...>
{
	static thread_task get_task()
	{
		auto task = task_list<head>::get_task();
		if (task) {
			return std::move(task);
		}

		return task_list<tails...>::get_task();
	}
};

class package_task : std::enable_shared_from_this<package_task>
{
public:
	virtual ~package_task() {}
	virtual void operator()() {}
};

using package_task_ptr = std::shared_ptr<package_task>;

template <size_t N, size_t index, typename task_vector, typename last, typename current>
struct task_selector_imp
{
	static bool selector(task_vector &v, void *&selector)
	{
		using next = decltype(std::get<N - index + 1>(v));
		selector = task_selector_imp<N, index - 1, task_vector, current, next>::selector;
		std::get<N - index>(v)();

		return true;
	}
};

template <size_t N, size_t index, typename task_vector, typename type>
struct task_selector_imp<N, index, task_vector, type, type>
{
	static bool selector(task_vector &v, void *&selector)
	{

	}
};

template <size_t N, typename task_vector, typename last, typename current>
struct task_selector_imp<N, 1, task_vector, last, current>
{
	static bool selector(task_vector &v, void *&selector)
	{
		selector = nullptr;
		std::get<N - 1>(v)();
		return false;
	}
};

template <typename type, typename ...task_types>
class sync_task : public package_task
{
	using task_vector = std::tuple<task_types...>;
	using task_selector = bool (*)(task_vector &, void *&);
public:
	sync_task(task_types &&...tasks) : tasks_(std::move(tasks)...)
	{
		selector_ = task_selector_imp<sizeof...(task_types), 
			sizeof...(task_types), 
			task_vector,
			type,
			decltype(std::get<0>(tasks_))
		>::selector;
	}

	virtual void operator()() 
	{
		bool has_next = true;
		while (has_next && selector_) {
			has_next = selector_(tasks_, (void *&)selector_);
		}
	}

private:
	task_selector selector_;
	task_vector tasks_;
};