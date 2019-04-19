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

	static std::shared_ptr<res> parse(binary_ptr &&bin);

	std::string& name()
	{
		return name_;
	}

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
