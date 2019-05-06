#pragma once

#include "res.hpp"
#include "binary.hpp"

#include <memory>

class block
{
public:
	struct header
	{
		header() = default;
		header(uint32_t ver) : id_('\0bk\0'), ver_(ver), name_(0), value_(0)
		{

		}

		uint32_t id_;
		uint32_t ver_;
		uint32_t name_;
		uint32_t value_;
	};

	enum node_flag
	{
		size_mask = 0x000F,
		type_mask = 0x00F0,

		type_bool = 0x0010,
		type_int = 0x0020,
		type_float = 0x0030,
		type_double = 0x0040,
		type_str = 0x0050,
		type_block = 0x0060,

		sorted = 0x0100,
		// type flag
	};

	struct node
	{
		node() = default;
		node(int32_t name, int32_t value) 
			: flag_(0), name_(name), value_(value), child_(0), child_num_(0)
		{
		}

		inline bool is_valid()
		{
			return value_ != 0;
		}

		int32_t flag_; // 0 ~ 7 bits, type index, 8 ~31 bits, control flags
		int32_t name_;
		int32_t value_;
		int32_t child_;
		int32_t child_num_;
	};

	class sub_block
	{
	public:
		sub_block(node* nodes = nullptr, const char* name = nullptr, const char* value = nullptr)
			: nodes_(nodes), name_(name), value_(value)
		{
		}

		inline const char* name()
		{
			return name(nodes_);
		}

		inline const char* name(node* node)
		{
			return name_ + node->name_;
		}

		inline const char* value()
		{
			return value(nodes_);
		}

		inline const char* value(node *n)
		{
			return value_ + n->value_;
		}

		inline sub_block read(const char* in_name)
		{
			auto child = nodes_ + nodes_->child_;
			for (int index = 0; index < nodes_->child_num_; ++index) {
				if (std::strcmp(name(child + index), in_name) == 0) {
					return sub_block(child + index, name_, value_);
				}
			}

			return sub_block();
		}

		inline sub_block read(size_t index)
		{
			if (index >= (size_t)nodes_->child_num_) {
				return sub_block();
			}

			auto child = nodes_ + nodes_->child_ + index;
			return sub_block(child, name_, value_);
		}

		inline bool is_valid()
		{
			return nodes_ != nullptr;
		}

		template <typename type>
		type as()
		{
		}

	public:
		node* nodes_;
		const char* name_;
		const char* value_;
	};

public:
	block(const binary_ptr& bin) : bin_(bin)
	{
		header* h = (header*)bin->get_data();

		root_.nodes_ = (node*)(bin->get_data() + sizeof(block::header));
		root_.name_ = bin->get_data() + h->name_;
		root_.value_ = bin->get_data() + h->value_;
	}

	inline sub_block& root()
	{
		return root_;
	}

	binary_ptr& get_bin()
	{
		return bin_;
	}

private:
	sub_block root_;
	binary_ptr bin_;
};

using block_ptr = std::shared_ptr<block>;


block_ptr to_block(const res_ptr &res);