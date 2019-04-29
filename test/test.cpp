
#include <iostream>

#include <res_mgr/stream.hpp>
#include <res_mgr/file_system.hpp>

#include <res_mgr/block.hpp>

int main(int argc, const char **argv)
{

	auto config = local_file_system::load_config("test.txt");

	auto block = to_block(config);

	auto name = block->root().name();

	block::sub_block sb = block->root().read("block");
	auto sb_name = sb.name();
	auto sb_value = sb.value();

	auto models = block->root().read("models");

	auto mn = models.name();
	auto mv = models.value();

	for (int i = 0; i < 10; ++i) {
		auto child = models.read(i);
		if (child.is_valid()) {
			auto sub_name = child.name();
			auto value = child.value();
			int i = 3;
		}
		else {
			break;
		}
	}
}