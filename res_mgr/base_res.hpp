#pragma once

#include "res.hpp"

#include <map>
#include <vector>
#include <memory>

class base_res : public res
{
public:
	using base_res_ptr = std::shared_ptr<base_res>;
	using node_type = std::pair<std::string, base_res_ptr>;
	using res_vector = std::vector<node_type>;

public:
	base_res(const std::string& name) : res(name)
	{
	}

	void add_child(const std::string &name, const base_res_ptr& ptr)
	{
		childs_.push_back(std::make_pair(name, ptr));
	}

private:
	res_vector childs_;
};

using base_res_ptr = base_res::base_res_ptr;
