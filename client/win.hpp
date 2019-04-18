#pragma once

#include <memory>

class win
{
public:
	virtual ~win() {}

	virtual void process_event() = 0;

	virtual size_t width() = 0;
	virtual size_t height() = 0;

	virtual void swap_buffers() = 0;
};

using win_ptr = std::shared_ptr<win>;
