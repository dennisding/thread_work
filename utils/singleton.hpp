#pragma once

template <typename type>
class singleton
{
protected:
	singleton() {}
public:
	singleton &operator=(const singleton &s) = delete;

	static type &instance()
	{
		static type ins_;
		return ins_;
	}
};
