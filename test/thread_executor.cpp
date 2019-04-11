
#include "thread_task.hpp"
#include "thread_context.hpp"

bool need_switch(thread_context &task)
{
	return task.need_switch();
}