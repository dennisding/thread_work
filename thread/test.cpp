
#include "thread.hpp"

#include <iostream>

int main(int argc, const char **argv)
{
	thread::init();

	thread::sync<thread::any>(
	[]() {
		std::cout << "task 1" << std::endl;
	},
	[]() {
		std::cout << "task 2" << std::endl;
	},
	[]() {
		std::cout << "task 3" << std::endl;
	}
	);
}