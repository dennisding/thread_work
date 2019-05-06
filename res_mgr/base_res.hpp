#pragma once

//#include "res.hpp"
//
//#include <map>
//#include <vector>
//#include <memory>
////
//class base_res : public res
//{
//public:
//	using base_res_ptr = std::shared_ptr<base_res>;
//	using res_vector = std::vector<base_res_ptr>;
//
//public:
//	inline base_res(const std::string& name) noexcept: res(name), value_(name), sorted_(true)
//	{
//	}
//
//	inline void add_child(const base_res_ptr& ptr)
//	{
//		childs_.push_back(ptr);
//	}
//
//	inline void set_value(std::string&& value)
//	{
//		value_.operator=(std::forward<std::string>(value));
//	}
//
//	inline void set_sorted(bool sort = true)
//	{
//		sorted_ = sort;
//	}
//
//	void ready()
//	{
//		if (sorted_) {
//			std::sort(childs_.begin(), childs_.end(), 
//				[](res_ptr && first, res_ptr && last) {
//					return first->name() < last->name();
//				});
//		}
//	}
//
//	// virtual method implements
//	virtual res_ptr read(const char *name)
//	{
//		if (sorted_) {
//			auto lower = std::lower_bound(childs_.begin(), childs_.end(), name,
//				[](const res_ptr &first, const char *name) -> bool {
//					return first->name() < name;
//				});
//			if (lower != childs_.end()) {
//				return *lower;
//			}
//		}
//		else {
//			for (auto& it : childs_) {
//				if (it->name() == name) {
//					return it;
//				}
//			}
//		}
//		
//		return nullptr;
//	}
//
//	virtual res_ptr read(size_t index)
//	{
//		if (index < childs_.size()) {
//			return childs_[index];
//		}
//
//		return nullptr;
//	}
//
//	virtual bool as_bool()
//	{
//		return !(value_ == "false" || value_ == "0" || value_.empty());
//	}
//
//	virtual int as_int()
//	{
//		return std::stoi(value_);
//	}
//
//	virtual float as_float()
//	{
//		return std::stof(value_);
//	}
//
//	virtual double as_double()
//	{
//		return std::stod(value_);
//	}
//
//	virtual std::string &as_string()
//	{
//		return value_;
//	}
//
//	virtual bool sorted()
//	{
//		return sorted_;
//	}
//
//private:
//	std::string value_;
//	res_vector childs_;
//	bool sorted_;
//};
//
//using base_res_ptr = base_res::base_res_ptr;
