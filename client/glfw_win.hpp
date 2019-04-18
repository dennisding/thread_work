#pragma once

#include "win.hpp"

#include <string>
#include <GLFW/glfw3.h>

class glfw_win : public win
{
public:
	glfw_win();
	~glfw_win();

	bool init(const std::string &title, size_t width, size_t height);

	virtual size_t width();
	virtual size_t height();

private:
	std::string title_;
	size_t width_;
	size_t height_;

	GLFWwindow *win_;
};
