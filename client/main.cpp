
#include "client.hpp"
#include "glfw_win.hpp"

#include <res_mgr/file_system.hpp>

int main(int argc, const char **argv)
{
	auto config = local_file_system::load_config("config.res");

	auto display = config->read("display");

	auto width = display->read<int>("width");
	auto height = display->read<int>("height");

	auto title = display->read<std::string>("title");

	auto ok = display->read<bool>("ok");

	// init
	// make glfw window
	auto win = std::make_shared<glfw_win>();
	win->init(title, width, height);

	client c;

	c.init(win);

	while (!c.is_quit()) {
		c.tick();
	}
}