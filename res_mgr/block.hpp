#pragma once

#include "binary.hpp"

#include <memory>

class block
{
public:
	struct header
	{
		//header(int32_t id = '\0bk\0', int32_t node_size = 0) : id_(id), node_size_(node_size)
		//{
		//}
		header() = default;
		header(int32_t size) : id_('\0bk\0'), node_size_(size)
		{

		}

		int32_t id_;
		int32_t node_size_;
	};

	struct node
	{
		node() = default;
		node(int32_t value) : value_(value), child_(0)
		{
		}

		inline bool is_valid()
		{
			return value_ != 0;
		}

		int32_t value_;
		int32_t child_;
	};

	struct value
	{
		const char* name_ = nullptr;
		const char* value_ = nullptr;
	};

	class sub_block
	{
	public:
		sub_block(const char* data = nullptr, node* nodes = nullptr)
			: data_(data), nodes_(nodes)
		{
			data_ = data;
		}

		inline const char* name()
		{
			return name(nodes_);
		}

		inline const char* name(node* node)
		{
			return data_ + node->value_ + sizeof(int32_t);
		}

		inline const char* value()
		{
			return value(nodes_);
		}

		inline const char* value(node *n)
		{
			auto value_offset = *((int32_t *)(data_ + n->value_));

			return data_ + n->value_ + value_offset;
		}

		inline sub_block read(const char* in_name)
		{
			auto child = nodes_ + nodes_->child_;
			while (child->is_valid()) {
				// check the child name
				if (std::strcmp(name(child), in_name) == 0) {
					return sub_block(data_, child);
				}
				child = child + 1;
			}

			return sub_block();
		}

		inline sub_block read(size_t index)
		{
			auto child = nodes_ + nodes_->child_ + index;
			if (child->is_valid()) {
				return sub_block(data_, child);
			}
			return sub_block();
		}

		inline bool is_valid()
		{
			return data_ != nullptr;
		}

		template <typename type>
		type as()
		{
		}

	public:
		const char* data_;
		node* nodes_;
	};

public:
	block(const binary_ptr& bin) : bin_(bin)
	{
		root_.nodes_ = (node *)(bin_->get_data() + sizeof(header));
		header* h = (header *)bin->get_data();

		// offset = header + offset
		root_.data_ = bin_->get_data() + sizeof(node)*h->node_size_;
	}

	sub_block* read(const char* name)
	{
	}

public:
	sub_block root_;

private:
	binary_ptr bin_;
};

using block_ptr = std::shared_ptr<block>;


