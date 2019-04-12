
#include <iostream>

template <typename t>
void print(t v);

void do_something()
{
	print(1);
}

template <typename t>
void print(t v)
{
	std::cout << "print value " << v << std::endl;
}

int main(int argc, const char **argv)
{
	do_something();
}