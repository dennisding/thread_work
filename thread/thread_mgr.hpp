#pragma once

#include "config.hpp"
#include "task_list.hpp"

#include <vector>
#include <thread>
#include <functional>

THREAD_NS_BEGIN

class thread_mgr
{
	using thread_vector = std::vector<std::thread *>;
	
	thread_mgr() : quit_(false)
	{

	}

public:
	inline static thread_mgr &instance()
	{
		static thread_mgr ins_;
		return ins_;
	}

	template <typename type, typename ...task_types>
	void sync(task_types ...tasks)
	{
		task_list<type>::instance().add_task(tasks...);
	}

	template <typename ...types>
	void add_worker()
	{
		auto t = new std::thread(&worker<types...>, this);
		threads_.push_back(t);
	}

	void quit()
	{
		quit_ = true;
	}

	bool is_quit()
	{
		return quit_;
	}

private:
	template <typename ...types>
	static void worker(thread_mgr *mgr)
	{
		while (!mgr->is_quit()) {
			task_list<types...>::instance().execute();
			std::this_thread::yield();
		}
	}

private:
	bool quit_;
	thread_vector threads_;
};

template <typename type, typename ...task_types>
inline void sync_imp(task_types &...tasks)
{
	thread_mgr::instance().sync<type>(std::move(tasks)...);
}

THREAD_NS_END
