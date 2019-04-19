#pragma once

#include "res.hpp"

#include <map>
#include <vector>
#include <memory>

class base_res : public res
{
public:
	using base_res_ptr = std::shared_ptr<base_res>;
	using res_vector = std::vector<base_res_ptr>;

public:
	base_res(const std::string& name) : res(name)
	{
	}

	void add_child(const base_res_ptr& ptr)
	{
		childs_.push_back(ptr);
	}

	void set_value(std::string &&value)
	{
		value_.operator=(std::forward<std::string>(value));
	}

private:
	std::string value_;
	res_vector childs_;
};

using base_res_ptr = base_res::base_res_ptr;
