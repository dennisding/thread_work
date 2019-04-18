#pragma once

#include "config.hpp"
#include "thread_mgr.hpp"

THREAD_NS_BEGIN

void init();
void setup();


template <typename type, typename ...task_types>
inline void sync(task_types &&...tasks)
{
	thread_mgr::instance().sync<type>(std::forward<task_types>(tasks)...);
}

template <typename type, typename ...task_types>
inline void sync_wait(task_types &&...tasks)
{
	thread_mgr::instance().sync_wait<type>(std::forward<task_types>(tasks)...);
}

template <typename ...types>
inline void tick()
{
	thread_mgr::instance().tick<types...>();
}

THREAD_NS_END
