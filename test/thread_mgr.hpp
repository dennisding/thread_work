#pragma once

#include "thread_task.hpp"

#include <vector>
#include <thread>

struct thread
{
	struct main {};
	struct io {};
	struct render {};
	struct logic {};

	template <typename type, typename task_type>
	struct work_at_imp
	{
		work_at_imp(task_type &&task) : task_(task)
		{
		}

		inline void operator()() { task_(); }

		task_type task_;
	};

	template <typename type, typename task_type>
	static work_at_imp<type, task_type> &&work_at(task_type &&task)
	{
		return work_at_imp<type, task_type>(std::move(task));
	}
};

class thread_mgr
{
	using thread_vector = std::vector<std::thread *>;
public:
	static thread_mgr &instance()
	{
		static thread_mgr ins_;
		return ins_;
	}

	void init();

	bool quit() { return quit_; }
	bool quit(bool is_quit) { quit_ = is_quit; }

	template <typename type>
	void tick()
	{
		auto task = task_list<type>::get_task();
		while (task) {
			task();
			task = task_list<type>::get_task();
		}
	}

	template <typename type, typename ...task_types>
	void sync(task_types &&... tasks)
	{
		sync_task<type, task_types...> t(std::move(tasks)...);
		//auto task  = std::make_shared<sync_task<task_types...>>(tasks...);

		t();
	}

private:

	thread_mgr() : quit_(false)
	{

	}

	template <typename ...types>
	static void worker(thread_mgr *mgr)
	{
		while (!mgr->quit()) {
			// execute one task
			auto task = task_list<types...>::get_task();
			if (task) {
				task();
			}
		}
	}

	template <typename ...types>
	void add_worker()
	{
		//auto t = std::make_shared<std::thread>(&worker<types...>, this);
		auto woker_fun = &worker<types...>;
		auto t = new std::thread(woker_fun, this);

		threads_.push_back(t);
	}

private:
	bool quit_;
	thread_vector threads_;
};
