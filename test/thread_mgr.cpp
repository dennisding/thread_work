#include "thread_mgr.hpp"

void thread_mgr::init()
{
	add_worker<thread::io>();
}