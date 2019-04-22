#pragma once

#include "res_config.hpp"

#include <string>

template <>
struct res_type<bool>
{
	static bool as(res* value)
	{
		return value->as_bool();
	}
};

template <>
struct res_type<int>
{
	static int as(res *value)
	{
		return value->as_int();
	}
};

template <>
struct res_type<float>
{
	static float as(res *value) 
	{
		return value->as_float();
	}
};

template <>
struct res_type<double>
{
	static double as(res* value)
	{
		return value->as_double();
	}
};

template <>
struct res_type<std::string>
{
	static std::string as(res* value)
	{
		return value->as_string();
	}
};