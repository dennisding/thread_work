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

	void set_value(std::string&& value)
	{
		value_.operator=(std::forward<std::string>(value));
	}

	// virtual method implements
	virtual res_ptr read(const char *name)
	{
		for (auto& it : childs_) {
			if (it->name() == name) {
				return it;
			}
		}
		return nullptr;
	}

	virtual res_ptr read(size_t index)
	{
		if (index < childs_.size()) {
			return childs_[index];
		}

		return nullptr;
	}

	virtual bool as_bool()
	{
		return !(value_ == "false" || value_ == "0");
	}

	virtual int as_int()
	{
		return std::stoi(value_);
	}

	virtual float as_float()
	{
		return std::stof(value_);
	}

	virtual double as_double()
	{
		return std::stod(value_);
	}

	virtual std::string &as_string()
	{
		return value_;
	}

private:
	std::string value_;
	res_vector childs_;
};

using base_res_ptr = base_res::base_res_ptr;
