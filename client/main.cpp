
#include "client.hpp"
#include "glfw_win.hpp"

#include <res_mgr/file_system.hpp>

int main(int argc, const char **argv)
{
	auto config = local_file_system::load_config("config.res");

	auto win_config = config->read("win");
	int width = win_config->read<int>("width");
	int height = win_config->read<int>("height");
	std::string title = win_config->read<std::string>("title");

	auto  test = config->read("test");

	auto t = test->as_tuple<std::string, std::string, std::string>();

	auto child = test->read_childs("aa");

	// make glfw window
	auto win = std::make_shared<glfw_win>();
	win->init(title, width, height);

	client c;

	c.init(win);

	while (!c.is_quit()) {
		c.tick();
	}
}