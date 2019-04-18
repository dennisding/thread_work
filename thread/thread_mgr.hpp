#pragma once

#include "config.hpp"
#include "task_list.hpp"

#include <vector>
#include <thread>
#include <future>
#include <functional>

THREAD_NS_BEGIN

class thread_mgr
{
	using thread_vector = std::vector<std::thread *>;
	
	inline thread_mgr() : quit_(false)
	{

	}

public:
	inline static thread_mgr &instance()
	{
		static thread_mgr ins_;
		return ins_;
	}

	template <typename type, typename ...task_types>
	inline void sync(task_types &&...tasks)
	{
		task_list<type>::instance().add_task(std::forward<task_types>(tasks)...);
	}

	template <typename type, typename ...task_types>
	inline void sync_wait(task_types &&...tasks)
	{
		std::promise<bool> wait;
		auto future = wait.get_future();

		task_list<type>::instance().add_task(std::forward<task_types>(tasks)...,
			[&wait]() { wait.set_value(true);  });

		future.get();
	}

	template <typename type>
	inline void dispatch(task *task)
	{
		task_list<type>::instance().dispatch(task);
	}

	template <typename ...types>
	inline void add_worker()
	{
		task_list<types...>::instance().execute();
		auto t = new std::thread(&worker<types...>, this);
		threads_.push_back(t);
	}

	template <typename ...types>
	inline void add_worker(int num)
	{
		for (int i = 0; i < num; ++i) {
			add_worker<types...>();
		}
	}

	inline void quit()
	{
		quit_ = true;
	}

	inline bool is_quit()
	{
		return quit_;
	}

	template <typename ...types>
	inline void tick()
	{
		task_list<types...>::instance().execute();
	}

private:
	template <typename ...types>
	static void worker(thread_mgr *mgr)
	{
		while (!mgr->is_quit()) {
			task_list<types...>::instance().execute();
			std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
	}

private:
	bool quit_;
	thread_vector threads_;
};

template <typename type, typename ...task_types>
inline void sync_imp(task_types &&...tasks)
{
	thread_mgr::instance().sync<type>(std::forward<task_types>(tasks)...);
}

template <typename type>
inline void dispatch(task *task)
{
	thread_mgr::instance().dispatch<type>(task);
}

THREAD_NS_END
