#pragma once

struct thread
{
	struct main {};
	struct io {};
	struct render {};
	struct logic {};
	struct any {};

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
	static work_at_imp<type, task_type> work_at(task_type &&task)
	{
		return work_at_imp<type, task_type>(std::move(task));
	}

	//template <typename last, typename current>
	//static last get_working_thread(const current &value)
	//{
	//	return last();
	//}

	//template <typename last, typename working_type, typename task>
	//static working_type get_working_thread(const work_at_imp<working_type, task> &t)
	//{
	//	return working_type();
	//}

	template <typename working, typename type>
	static working get_working_thread(type t)
	{
		return working();
	}

	template <typename working, typename working_type, typename task>
	static working_type get_working_thread(work_at_imp<working_type, task> t)
	{

	}
};