#pragma once

#include "res_config.hpp"

#include <memory>
#include <algorithm>

class binary
{
public:
	inline binary(size_t size) : size_(size)
	{
		data_ = new char[size + 1];
		data_[size] = 0;
	}

	inline binary(binary &&bin)
	{
		size_ = bin.size_;
		data_ = bin.data_;

		bin.size_ = 0;
		bin.data_ = nullptr;
	}

	inline ~binary()
	{
		delete[] data_;
	}

	inline binary &operator=(binary &&bin)
	{
		std::swap(size_, bin.size_);
		std::swap(data_, bin.data_);
	}

	inline char *get_data()
	{
		return data_;
	}

	inline size_t size()
	{
		return size_;
	}

private:
	size_t size_;
	char *data_;
};

using binary_ptr = std::shared_ptr<binary>;

template <>
struct res_info<binary>
{
	static binary_ptr parse(binary_ptr && bin) 
	{
		return std::move(bin);
	}

	static binary_ptr parse(binary_ptr& bin)
	{
		return bin;
	}
};
