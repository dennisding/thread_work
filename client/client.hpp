#pragma once

#include "win.hpp"

#include <utils/singleton.hpp>

class client : public singleton<client>
{
public:
	client();

	void init(const win_ptr &win);
	void tick();

	bool is_quit() 
	{
		return quit_;
	}

private:
	win_ptr win_;
	bool quit_;
};

