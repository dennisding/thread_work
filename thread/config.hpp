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

// forward definition
class task;

template <typename type, typename ...task_types>
void sync_imp(task_types &&...tasks);

template <typename type>
void dispatch(std::shared_ptr<task> &&task);


THREAD_NS_END