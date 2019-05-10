#pragma once
//
//#include "res_config.hpp"
//
//#include <string>
//
//template <typename type>
//class res_type_imp : public res_type
//{
//public:
//	type convert(const std::string& value)
//	{
//		return res_type_info<type>::convert(value);
//	}
//
//	type convert(const char* value)
//	{
//		return res_type_info<type>::convert(value);
//	}
//
//	virtual int as_int(const std::string& value)
//	{
////		return res_type_info<int>::convert(value);
//		return do_convert<type, int>(value);
//	}
//
//	virtual std::string as_string(const std::string& value)
//	{
////		return res_type_info<std::string>::convert(value);
//		return do_convert<type, std::string>(value);
//	}
//
//	virtual int as_int(const char* value)
//	{
//		//return res_type_info<int>::convert(value);
//		return do_convert<type, int>(value);
//	}
//
//	virtual std::string as_string(const char* value)
//	{
////		return res_type_info<int>::convert(value);
//		return do_convert<type, std::string>(value);
//	}
//
//	virtual const char* as_cstr(const char* value)
//	{
//		return value;
//	}
//
//private:
//	template <typename type, typename return_type, typename arg_type>
//	return_type do_convert(arg_type &&arg)
//	{
//		return res_type_info<return_type>::convert(std::forward<arg_type>(arg));
//	}
//};
//
//
//// type infos
//template <>
//struct res_type_info<int> : public res_type_imp<int32_t>
//{
//	static int convert(const std::string& value)
//	{
//		return std::stoi(value);
//	}
//
//	static int32_t convert(const char* value)
//	{
//		return *((int32_t*)value);
//	}
//};
//
//template <>
//struct res_type_info<std::string> : public res_type_imp<std::string>
//{
//	static const std::string convert(const std::string& value)
//	{
//		return value;
//	}
//
//	static const std::string convert(const char* value)
//	{
//		return value;
//	}
//};
