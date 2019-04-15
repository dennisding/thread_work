#include "thread.hpp"

THREAD_NS_BEGIN

void init()
{

}

void setup()
{
	thread_mgr::instance().add_worker<io>();
	thread_mgr::instance().add_worker<any>();
	thread_mgr::instance().add_worker<any>();
	thread_mgr::instance().add_worker<render>();
	thread_mgr::instance().add_worker<script>();
}

THREAD_NS_END