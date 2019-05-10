#pragma once

#include "binary.hpp"

#include <memory>
#include <string>
#include <vector>

// [b]lock
// [i]nfomation
// [t]ext
class bit
{
public:
	bit(const std::string& name) : name_()
	{
	}

	const std::string& name()
	{
		return name_;
	}

	const std::string& type()
	{
		return type_;
	}

	template <typename type>
	type as(size_t index = 0)
	{
		return type_info<type>::convert(this, index);
	}

	virtual void set_value(const std::string& vlaue, size_t index = 0) = 0;
	virtual int as_int(size_t index = 0) = 0;

private:
	std::string name_;
	std::string type_;
};

using bit_ptr = std::shared_ptr<bit>;

template <typename type, size_t size>
class bit_primary : public bit
{
	using value_vector = std::vector<type>;
public:


private:
	value_vector values_;
};

class bit_block : public bit
{
public:

private:
	bit_ptr value_;
};


bit_ptr to_bit(binary_ptr &bin);
