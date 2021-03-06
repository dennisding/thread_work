#pragma once

#include "config.hpp"

#include <tuple>

THREAD_NS_BEGIN

class context;

template <typename type, typename task_type, int has_context>
class work_at_imp
{
	using store_type = typename std::remove_reference<task_type>::type;
public:
	inline work_at_imp(task_type &&task) : task_(std::forward<task_type>(task))
	{
	}

	inline void operator()()
	{
		task_();
	}


private:
	store_type task_;
};

template <typename type, typename task_type>
class work_at_imp<type, task_type, 1>
{
	using store_type = typename std::remove_reference<task_type>::type;
public:
	inline work_at_imp(task_type &&task) : task_(std::forward<task_type>(task))
	{
	}

	inline void operator()(context &c)
	{
		task_(c);
	}


private:
	store_type task_;
};

template <typename type, typename task_type>
work_at_imp<type, task_type, has_context<task_type>::value> work_at(task_type &&task)
{
	using work_at_type = work_at_imp<type, task_type, has_context<task_type>::value>;
	return work_at_type(std::forward<task_type>(task));
}

template <typename working_type, typename task_type>
struct get_working_type
{
public:
	using type = working_type;
};

template <typename working_type, typename work_at_type, typename task_type, int has_context>
struct get_working_type<working_type, work_at_imp<work_at_type, task_type, has_context>>
{
public:
	using type = work_at_type;
};


template <typename tuple, typename task>
class executor
{
public:
	inline executor &instance()
	{
		static executor ins_;
		return ins_;
	}

	virtual bool execute(tuple &t, executor *&e, task *task)
	{
		// final task
		delete task;
		return true;
	}
};

template <typename tuple, typename task_type, int size, int index, typename last, typename current>
class executor_imp : public executor<tuple, task_type>
{
public:
	inline static executor_imp &instance()
	{
		static executor_imp ins_;
		return ins_;
	}

	virtual bool execute(tuple &t, executor<tuple, task_type> *&e, task_type *task)
	{
		// work at different working thread, dispatch the task
		e = &executor_imp<tuple, task_type, size, index, current, current>::instance();
		dispatch<current>(task);
		return true;
	}
};

template <typename tuple, typename task_type, int size, int index, typename current>
class executor_imp<tuple, task_type, size, index, current, current>
	: public executor<tuple, task_type>
{
public:
	inline static executor_imp &instance()
	{
		static executor_imp ins_;
		return ins_;
	}

	virtual bool execute(tuple &t, executor<tuple, task_type> *&e, task_type *task)
	{
		// match with last working thread
		using real_current = typename std::tuple_element<size - index, tuple>::type;
		using next = typename std::tuple_element<size - index + 1, tuple>::type;
		e = &executor_imp<tuple, task_type, size, index - 1, real_current::type, next::type>::instance();

		// execute the task
		return !std::get<size - index>(t)(task);
	}
};

template <typename tuple, typename task_type, int size, typename last, typename current>
class executor_imp<tuple, task_type, size, 1, last, current>
	: public executor<tuple, task_type>
{
public:
	static executor_imp &instance()
	{
		static executor_imp ins_;
		return ins_;
	}

	virtual bool execute(tuple &t, executor<tuple, task_type> *&e, task_type *task)
	{
		/// final task in different working thread
		e = &executor_imp<tuple, task_type, size, 1, current, current>::instance();
		dispatch<current>(task);
		return true;
	}
};

template <typename tuple, typename task_type, int size, typename current>
class executor_imp<tuple, task_type, size, 1, current, current>
	: public executor<tuple, task_type>
{
public:
	static executor_imp &instance()
	{
		static executor_imp ins_;
		return ins_;
	}

	virtual bool execute(tuple &t, executor<tuple, task_type> *&e, task_type *task)
	{
		// final task in same working thread
		// the work is done!
		e = &executor<tuple, task_type>::instance();

		if (std::get<size - 1>(t)(task)) {
			delete task;
		}

		return true;
	}
};

template <typename tuple, typename task, typename type>
executor<tuple, task> *get_executor()
{
	return &executor_imp<tuple, 
		task, 
		std::tuple_size<tuple>::value, 
		std::tuple_size<tuple>::value,
		type,
		type
	>::instance();
}

THREAD_NS_END