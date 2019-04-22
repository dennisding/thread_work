#pragma once

#include "res_config.hpp"

#include "binary.hpp"

#include <string>
#include <memory>

class res
{
public:
	res(const std::string& name) : name_(name)
	{
	}

	res(std::string&& name) : name_(std::move(name))
	{
	}

	virtual ~res() {}

	std::string& name()
	{
		return name_;
	}

	template <typename type>
	type as()
	{
		return res_type<type>::as(this);
	}

	template <typename type>
	type read(const std::string& name)
	{
		return read(name.c_str())->as<type>();
	}

	template <typename type>
	type read(const char* name)
	{
		return read(name)->as<type>();
	}

	// virtual base implment
	virtual std::shared_ptr<res> read(const char *name) = 0;

	virtual bool as_bool()
	{
		auto value = as_string();

		return !(value == "false" || value == "0");
	}

	virtual int as_int()
	{
		return std::stoi(as_string());
	}

	virtual float as_float()
	{
		return std::stof(as_string());
	}

	virtual double as_double()
	{
		return std::stod(as_string());
	}

	virtual std::string &as_string()
	{
		return name_;
	}

public:
	static std::shared_ptr<res> parse(binary_ptr&& bin);

private:
	std::string name_;
};

using res_ptr = std::shared_ptr<res>;

template <>
struct res_info<res>
{
	static res_ptr parse(binary_ptr&& bin)
	{
		return res::parse(std::forward<binary_ptr>(bin));
	}
};

#include "res_types.hpp"
