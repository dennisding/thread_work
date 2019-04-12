#pragma once

#include "config.hpp"

#include <tuple>

THREAD_NS_BEGIN

template <typename tuple, typename task>
class executor
{
public:
	virtual bool execute(tuple &t, executor *&e, task *task)
	{
		return true;
	}
};

template <typename tuple, typename task, int size, int index, typename last, typename current>
class executor_imp : public executor<tuple, task>
{
public:
	inline static executor_imp &instance()
	{
		static executor_imp ins_;
		return ins_;
	}

	virtual bool execute(tuple &t, executor<tuple, task> *&e, task *task)
	{
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
		std::get<size - index>(t)();

		return false;
	}
};

template <typename tuple, typename task, int size, typename last, typename current>
class executor_imp<tuple, task, size, 1, last, current>
	: public executor<tuple, task>
{
public:
	static executor_imp &instance()
	{
		static executor_imp ins_;
		return ins_;
	}

	virtual bool execute(tuple &t, executor<tuple, task> *&e, task *task)
	{
		// final task in different working thread
		std::get<size - 1>(t)();
		e = nullptr;
		return true;
	}
};

template <typename tuple, typename task, int size, typename current>
class executor_imp<tuple, task, size, 1, current, current>
	: public executor<tuple, task>
{
public:
	static executor_imp &instance()
	{
		static executor_imp ins_;
		return ins_;
	}

	virtual bool execute(tuple &t, executor<tuple, task> *&e, task *task)
	{
		// final task in same working thread
		std::get<size - 1>(t)();
		e = nullptr;
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