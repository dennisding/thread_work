
#include <iostream>

void test_fun()
{
}

template <typename type, typename ...types>
void test_fun(type t, types ...args)
{
	test_fun(args...);

	std::cout << t;
}

int main(int argc, const char **argv)
{
	test_fun(1, 2, 4);
}