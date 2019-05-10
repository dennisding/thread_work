
#include "bit.hpp"

#include <cassert>

struct bit_info
{
	using string_vector = std::vector<std::string>;
	using child_vector = std::vector<bit_ptr>;

	std::string name_;
	string_vector types_;
	string_vector options_;
	string_vector values_;
	child_vector childs_;

	bit_info(const std::string& name) : name_(name)
	{
	}
};

class bit_parser
{
	using bit_info_vector = std::vector<bit_info>;
public:
	bit_parser(binary_ptr& bin) : bin_(bin)
	{
	}

	bit_ptr parse()
	{
		if (!bin_) {
			return nullptr;
		}

		infos_.emplace_back(bit_info("root"));
		parse_block();

		return gen_bit();
	}

private:
	bit_ptr gen_bit()
	{
	}

	void parse_block()
	{
	}

private:
	binary_ptr bin_;
//	bit_info info_;
	bit_info_vector infos_;
};

bit_ptr to_bit(binary_ptr& bin)
{
	bit_parser parser(bin);

	return parser.parse();
}