#pragma once

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

THREAD_NS_END