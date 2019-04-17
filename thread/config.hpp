#pragma once

#include <memory>

#define THREAD_NS thread
#define THREAD_NS_BEGIN namespace THREAD_NS{
#define THREAD_NS_END }

THREAD_NS_BEGIN

// working type
struct io {};
struct render {};
struct script {};
struct main {};
struct any {};

class task
{
public:
	task() : next_(nullptr) {}
	virtual ~task() {}
	virtual void operator()() = 0;
	virtual void dispatch_self() = 0;

	task *next_;
};

using task_ptr = task * ;
//using task_ptr = std::shared_ptr<task>;


template <typename type, typename ...task_types>
void sync_imp(task_types &&...tasks);

//template <typename type>
//void dispatch(std::shared_ptr<task> &&task);

template <typename type>
void dispatch(task *task);


THREAD_NS_END