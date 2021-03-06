
#include "glfw_win.hpp"


glfw_win::glfw_win() :
	win_(nullptr)
{
}

glfw_win::~glfw_win()
{
	win_ = nullptr;
	glfwTerminate();
}

bool glfw_win::init(const std::string &title, size_t width, size_t height)
{
	title_ = title;
	width_ = width;
	height_ = height;

	if (!glfwInit()) {
		return false;
	}

	win_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!win_) {
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(win_);
	glfwSwapInterval(1);

	return true;
}

void glfw_win::process_event()
{
	glfwPollEvents();
}

void glfw_win::swap_buffers()
{
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(win_);
}