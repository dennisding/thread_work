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
	}
};

template <typename tuple, typename task, int size, int index, typename current>
class executor_imp<tuple, task, size, index, current, current> 
	: public executor<tuple, task>
{
	inline static executor_imp &instance()
	{
		static executor_imp ins_;
		return ins_;
	}

	virtual bool execute(tuple &t, executor<tuple, task> *&e, task *task)
	{
		// match with last working thread
//		e = &executor_imp<tuple, task, size, index - 1, real_current, next>::instance();

		// execute the task
		std::get<size - index>(t)();

		return true;
	}
};

template <typename tuple, typename task, int size, typename last, typename current>
class executor_imp<tuple, task, size, 0, last, current>
	: public executor<tuple, task>
{
	static executor_imp &instance()
	{
		static executor_imp ins_;
		return ins_;
	}

	virtual bool execute(tuple &t, executor<tuple, task> *&e, task *task)
	{
		// final task in different working thread
		e = nullptr;
		return false;
	}
};

template <typename tuple, typename task, int size, typename current>
class executor_imp<tuple, task, size, 0, current, current>
	: public executor<tuple, task>
{
	static executor_imp &instance()
	{
		static executor_imp ins_;
		return ins_;
	}

	virtual bool execute(tuple &t, executor<tuple, task> *&e, task *task)
	{
		// final task in same working thread
		e = nullptr;
		return false;
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