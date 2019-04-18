
#include "client.hpp"
#include "glfw_win.hpp"


int main(int argc, const char **argv)
{
	// init
	// make glfw window
	auto win = std::make_shared<glfw_win>();
	win->init("hello world!", 620, 480);

	client c;

	c.init(win);

	while (!c.is_quit()) {
		c.tick();
	}

}