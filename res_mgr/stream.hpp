#pragma once

#include "binary.hpp"

#include <string>
#include <type_traits>

class stream;

template <typename type>
struct stream_writer
{
	//static size_t write(stream* s, const type& value)
	//{
	//	// write some data to stream
	//	return 0;
	//}
};

class stream
{
	constexpr int block_size()
	{
		return 4096;
	}

	template <size_t value>
	struct fun_selector
	{
	};

public:
	stream() : index_(0), bin_(std::make_shared<binary>(block_size()))
	{
	}

	inline size_t get_pos()
	{
		return index_;
	}

	// write pod only
	// didn't reset the index
	template <typename type>
	size_t write_at(const type& value, size_t pos)
	{
		std::memcpy(bin_->get_data() + pos, &value, sizeof(value));
	}

	template <typename type>
	size_t write(const type &value)
	{
		return write(value, fun_selector<std::is_standard_layout<type>::value>());
	}

	size_t write(void* ptr, size_t size)
	{
		if (size + index_ > bin_->size()) {
			expand_bin(size);
		}
		std::memcpy(bin_->get_data() + index_, ptr, size);
		index_ += size;
		return size;
	}

	size_t write(const char* value)
	{
		size_t size = std::strlen(value);
		if (size + index_ > bin_->size()) {
			expand_bin(size);
		}

		std::memcpy(bin_->get_data() + index_, value, size);
		index_ += size;

		size_t indent = 4 - size % 4;
		for (size_t i = 0; i < indent; ++i) {
			write('\0');
		}

		return size + indent;
	}

	binary_ptr get_bin()
	{
		bin_->resize(index_);
//		return bin_;
		return std::move(bin_);
	}

	binary_ptr get_packed_bin()
	{
		auto result = std::make_shared<binary>(index_);
		std::memcpy(result->get_data(), bin_->get_data(), index_);

		return std::move(result);
	}

private:
	template <typename type>
	size_t write(const type& value, const fun_selector<0>& fun)
	{
		return stream_writer<type>::write(this, value);
	}

	template <typename type>
	size_t write(const type& value, const fun_selector<1>& fun)
	{
		size_t size = sizeof(type);

		if (size + index_ > bin_->size()) {
			expand_bin(size);
		}

		std::memcpy(bin_->get_data() + index_, &value, size);
		index_ += size;

		return size;
	}

private:
	void expand_bin(size_t data_size)
	{
		auto new_size = std::max(index_ + data_size, bin_->size() + block_size());
		bin_->resize(new_size);
	}

private:
	size_t index_;
	binary_ptr bin_;
};

template <>
struct stream_writer<std::string>
{
	static size_t write(stream* s, const std::string& value)
	{
		int32_t size = (int32_t)value.size();
		
		s->write(size);
		size_t str_size = s->write(value.c_str());
		
		return sizeof(int32_t) + str_size;
	}
};
