#pragma once

#include "res_config.hpp"

#include "binary.hpp"

#include <tuple>
#include <string>
#include <vector>
#include <memory>

class res
{
	template <size_t ...size>
	struct index_type
	{};

	template <typename type, size_t size>
	struct cat_index_type
	{
	};

	template <size_t ...types, size_t size>
	struct cat_index_type<index_type<types...>, size>
	{
		using type = index_type<types..., size>;
	};

	template <size_t size>
	struct make_index
	{
		using type = typename cat_index_type<typename make_index<size - 1>::type, size>::type;
	};

	template <>
	struct make_index<0>
	{
		using type = index_type<0>;
	};

	template <typename type, typename index>
	struct call_helper
	{
	};

	template <typename ...types, size_t ...indexs>
	struct call_helper<std::tuple<types...>, index_type<indexs...> >
	{
		using tuple = std::tuple<types...>;

		inline static tuple call(res* resource)
		{
			return std::make_tuple<types...>(do_call<types>(resource, indexs)...);
		}

		template <typename type>
		static inline type do_call(res *resource, size_t index)
		{
			auto ptr = resource->read(index);
			if (!ptr) {
				return type();
			}
			return ptr->as<type>();
		}
	};
private:
	using res_vector = std::vector<std::shared_ptr<res>>;

public:
	inline res(const std::string& name) noexcept: sorted_(false), name_(name)
	{
	}

	inline res(std::string&& name) noexcept: sorted_(false), name_(std::move(name))
	{
	}

	virtual ~res() {}

	inline std::string& name()
	{
		return name_;
	}

	std::string& value()
	{
		return value_;
	}

	void set_value(std::string && value)
	{
		value_.operator=(std::forward<std::string>(value));
	}

	template <typename type>
	inline type as()
	{
		return res_type_info<type>::convert(value_);
	}

	template <typename ...types>
	inline std::tuple<types...> as_tuple()
	{
		using tuple = std::tuple<types...>;
		using index_type = typename make_index<std::tuple_size<tuple>::value - 1>::type;
		return call_helper<tuple, index_type>::call(this);
	}

	template <typename type>
	inline std::vector<type> as_vector()
	{
		using vector = std::vector<type>;
	}

	template <typename type>
	inline type read(const std::string& name)
	{
		return read<type>(name.c_str());
	}

	template <typename type>
	inline type read(const char* name)
	{
		auto child = read(name);
		if (!child) {
			return type();
		}

		return child->as<type>();
	}

	template <typename type>
	inline type read(size_t index)
	{
		auto child = read(index);
		if (!child) {
			return type();
		}
		return child->as<type>();
	}

	inline  std::vector<std::shared_ptr<res>> read_childs(const std::string& name)
	{
		return read_childs(name.c_str());
	}

	std::vector<std::shared_ptr<res>> read_childs(const char *name)
	{
		size_t index = 0;
		std::vector<std::shared_ptr<res>> childs;

		for (auto& child : childs_) {
			if (child->name() == name) {
				childs.push_back(child);
			}
		}

		return std::move(childs);
	}

	void add_child(const std::shared_ptr<res>& child)
	{
		childs_.push_back(child);
	}

	virtual std::shared_ptr<res> read(const char* name)
	{
		if (sorted_) {
			auto lower = std::lower_bound(childs_.begin(), childs_.end(), name,
				[](const std::shared_ptr<res> & first, const char* name) -> bool {
					return first->name() < name;
				});
			if (lower != childs_.end()) {
				return *lower;
			}
		}
		else {
			for (auto& it : childs_) {
				if (it->name() == name) {
					return it;
				}
			}
		}

		return nullptr;
	}

	virtual std::shared_ptr<res> read(size_t index)
	{
		if (index >= childs_.size()) {
			return nullptr;
		}

		return childs_[index];
	}

	virtual bool as_bool()
	{
//		auto value = as_string();

		return !(value_ == "false" || value_ == "0" || value_.empty());
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

	// state interface
	virtual bool sorted()
	{
		return true;
	}

	void set_sorted(bool sorted = true)
	{
		sorted_ = sorted;
	}

	void ready()
	{
		if (sorted_) {
			std::sort(childs_.begin(), childs_.end(),
				[](std::shared_ptr<res> & first, std::shared_ptr<res> & last) {
					return first->name() < last->name();
				});
		}
	}

public:
	static std::shared_ptr<res> parse(binary_ptr&& bin);

private:
	bool sorted_;
	std::string name_;
	std::string value_;
	res_vector childs_;
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
