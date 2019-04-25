
#include <iostream>

#include <res_mgr/stream.hpp>
#include <res_mgr/file_system.hpp>

int main(int argc, const char **argv)
{

	auto config = local_file_system::load_config("test.txt");

	auto block = config->to_block();

	auto name = block->root_.name();

	block::sub_block sb = block->root_.read("block");

	auto models = block->root_.read("models");

	auto mn = models.name();

	for (int i = 0; i < 10; ++i) {
		auto child = models.read(i);
		if (child.is_valid()) {
			auto sub_name = child.name();
			auto value = child.value() + sizeof(int32_t);
			int i = 3;
		}
		else {
			break;
		}
	}
}