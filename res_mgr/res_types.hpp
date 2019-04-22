#pragma once

#include "res_config.hpp"

#include <string>

template <>
struct res_type<bool>
{
	inline static bool as(res* value)
	{
		return value->as_bool();
	}
};

template <>
struct res_type<int>
{
	inline static int as(res *value)
	{
		return value->as_int();
	}
};

template <>
struct res_type<float>
{
	inline static float as(res *value)
	{
		return value->as_float();
	}
};

template <>
struct res_type<double>
{
	inline static double as(res* value)
	{
		return value->as_double();
	}
};

template <>
struct res_type<std::string>
{
	inline static std::string as(res* value)
	{
		return value->as_string();
	}
};