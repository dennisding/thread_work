
#include <iostream>

#include <res_mgr/stream.hpp>
#include <res_mgr/file_system.hpp>

#include <res_mgr/block.hpp>

int main(int argc, const char **argv)
{

	auto config = local_file_system::load_config("test.txt");

	auto block1 = to_block(config);

	auto name = block1->root().name();

	block::sub_block sb = block1->root().read("block");
	auto sb_name = sb.name();
	auto sb_value = sb.value();

	auto models = block1->root().read("models");

	auto mn = models.name();
	auto mv = models.value();

//	local_file_system::write("test.bin", block->get_bin());
	auto bin = local_file_system::read_file("test.bin");
	auto b = block(bin);
	
	auto bk = b.root().read("block");
	std::cout << bk.name() << std::endl;
	std::cout << bk.value() << std::endl;
}
