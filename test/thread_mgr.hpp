#pragma once

#include "thread.hpp"

#include <vector>
#include <thread>

class thread_mgr
{
	using thread_vector = std::vector<std::shared_ptr<std::thread>>;
public:
	static thread_mgr &instance()
	{
		static thread_mgr ins_;
		return ins_;
	}

	void init();

	bool quit() { return quit_; }
	void quit(bool is_quit) { quit_ = is_quit; }

	template <typename type>
	void tick()
	{
	}

	template <typename type, typename ...task_types>
	void sync(task_types &&... tasks)
	{
		auto task = std::make_shared< sync_task<type, task_types...> >(std::move(tasks)...);

		task_list<type>::instance().add_task(task);
	}

	template <typename type>
	void dispatch(package_task *task)
	{
		task_list<type>::instance().add_task(task->shared_from_this());
	}

	void join()
	{
		for (auto it : threads_) {
			it->join();
		}
	}

private:

	thread_mgr() : quit_(false)
	{

	}

	template <typename ...types>
	static void worker(thread_mgr *mgr)
	{
		while (!mgr->quit()) {
			auto task = task_list<types...>::instance().get_task();
			if (task) {
				(*task)();
			}
//			else {
				std::this_thread::yield();
//			}
		}
	}

	template <typename ...types>
	void add_worker()
	{
		auto t = std::make_shared<std::thread>(&worker<types...>, this);
		threads_.push_back(t);
	}

private:
	bool quit_;
	thread_vector threads_;
};
