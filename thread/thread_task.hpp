#pragma once

#include "config.hpp"

THREAD_NS_BEGIN


class task : public std::enable_shared_from_this<task>
{
public:
	virtual ~task() {}
	virtual void operator()() = 0;
	virtual void dispatch_self() = 0;
};

using task_ptr = std::shared_ptr<task>;

THREAD_NS_END
