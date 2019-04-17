#include "thread.hpp"

THREAD_NS_BEGIN

void init()
{
}

void setup()
{
	// 3 io thread
	thread_mgr::instance().add_worker<io>(3);
	// work for any
	auto thread_num = std::thread::hardware_concurrency() * 2;
	if (thread_num == 0) {
		thread_num = 1;
	}
	thread_mgr::instance().add_worker<any>(thread_num);
	thread_mgr::instance().add_worker<render>();
	thread_mgr::instance().add_worker<script>();
}

THREAD_NS_END