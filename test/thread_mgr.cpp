#include "thread_mgr.hpp"

void thread_mgr::init()
{
	add_worker<thread::io>();
	add_worker<thread::any>();
	add_worker<thread::logic>();
	add_worker<thread::render>();
}