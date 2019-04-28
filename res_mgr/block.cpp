#include "block.hpp"

#include "res.hpp"
#include "stream.hpp"

#include <map>
#include <tuple>
#include <string>
#include <vector>

class block_packer
{
	using child_info_type = std::tuple<uint32_t, uint32_t>;
	using node_vector = std::vector<block::node>;
	using string_map = std::map<std::string, uint32_t>;
public:
	block_packer(res* res) : res_(res)
	{
	}

	block_ptr pack()
	{
		do_pack();

		stream result;
		// prepare the data
		auto names = names_.get_bin();
		auto values = values_.get_bin();

		// write header
		block::header header(1);
		header.name_ = sizeof(header) + sizeof(block::node) * nodes_.size();
		header.value_ = header.name_ + names->size();
		result.write(header);

		// write nodes
		result.write(&nodes_[0], sizeof(block::node)*nodes_.size());

		// write names
		result.write(names->get_data(), names->size());
		// write values
		result.write(values->get_data(), values->size());

		auto result_bin = result.get_bin();

		return std::make_shared<block>(result_bin);
	}

private:
	void do_pack()
	{
		// pack the root
		auto name_offset = write_name(res_);
		auto value_offset = write_value(res_);

		nodes_.push_back(block::node(name_offset, value_offset));

		auto child_info = pack_child(res_);

		nodes_[0].child_ = std::get<0>(child_info);
		nodes_[0].child_num_ = std::get<1>(child_info);
	}

	child_info_type pack_child(res* res)
	{
		uint32_t child_num = 0;
		uint32_t child_offset = nodes_.size();
		for (; child_num < std::numeric_limits<uint32_t>::max(); ++child_num) {
			auto child = res->read(child_num);
			if (!child) {
				break;
			}

			// pack child
			auto name_offset = write_name(child.get());
			auto value_offset = write_value(child.get());
			
			nodes_.push_back(block::node(name_offset, value_offset));
		}

		for (uint32_t index = 0; index < child_num; ++index) {
			auto child = res->read(index);
			if (!child) {
				break;
			}

			// pack child
			auto child_index = child_offset + index;
			auto child_info = pack_child(child.get());
			nodes_[child_index].child_ = std::get<0>(child_info) - child_index;
			nodes_[child_index].child_num_ = std::get<1>(child_info);
		}

		return std::make_tuple<uint32_t, uint32_t>(std::move(child_offset), std::move(child_num));
	}

	uint32_t write_name(res* res)
	{
		auto name = res->name();
		auto finded = name_map_.find(name);
		if (finded != name_map_.end()) {
			return finded->second;
		}

		auto pos = names_.get_pos();
		names_.write(name.c_str(), name.size() + 1); // add '\0' at the end
		name_map_[name] = pos;

		return pos;
	}

	uint32_t write_value(res* res)
	{
		auto pos = values_.get_pos();

		auto value = res->as<std::string>();
		values_.write(value.c_str(), value.size() + 1);

		return pos;
	}

private:
	node_vector nodes_;
	stream names_;
	string_map name_map_;
	stream values_;

	res* res_;
};

block_ptr to_block(const res_ptr& res)
{
	block_packer packer(res.get());

	return packer.pack();
}