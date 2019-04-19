#include "res.hpp"
#include "base_res.hpp"

#include <utils/singleton.hpp>

#include <cassert>
#include <vector>

class parser_info
{
public:
	enum token_type
	{
		BLOB,
		NEW_LINE,
		NUMBER,
		FLOAT,
		STR,
		TYPE_START,
		TYPE_END,
		STR_MARK,
		INDENT,
		SPACE,
		ASSIGN,
		COMMENT,
	};

	parser_info()
	{
		init_char_table();
	}

public:
	token_type char_table_[256];

private:
	void init_char_table()
	{
		memset(char_table_, 0, sizeof(char_table_));
		char_table_['\n'] = NEW_LINE;
		char_table_['\r'] = NEW_LINE;
		char_table_['{'] = TYPE_START;
		char_table_['}'] = TYPE_END;
		char_table_['\''] = STR_MARK;
		char_table_['\"'] = STR_MARK;
		char_table_['\t'] = INDENT;
		char_table_[' '] = SPACE;
		char_table_['='] = ASSIGN;
		char_table_['#'] = COMMENT;
	}
};

static parser_info g_parser_info;

class res_parser
{
	using res_vector = std::vector<std::shared_ptr<base_res>>;
	
public:
	res_parser(const binary_ptr& bin) : bin_(bin), root_(nullptr), index_(0), indent_(0)
	{
	}

	res_ptr parse()
	{
		if (!bin_) {
			return nullptr;
		}

		auto root = std::make_shared<base_res>("root");
		root_ = root;
		stack_.push_back(std::move(root));

		// parse the root
		parse_block();

		return root_;
	}

private:
	void parse_block()
	{
		while (index_ < bin_->size()) {
			auto index = index_;
			++index_;

			auto token = g_parser_info.char_table_[bin_->get_data()[index]];

			if (token == parser_info::BLOB) {
				feed_one_block(index);
			}
			else if (token == parser_info::COMMENT) {
				skip_to(parser_info::NEW_LINE);
			}
			else if (token == parser_info::ASSIGN) {

			}

			// reset the indent
			if (token == parser_info::INDENT) {
				++indent_;
			}
			else {
				indent_ = 0;
			}
		}
	}

	void skip_to(parser_info::token_type token)
	{
		auto data = bin_->get_data();
		while (index_ < bin_->size() && g_parser_info.char_table_[data[index_]] != token) {
			++index_;
		}
	}

	void eat(parser_info::token_type token)
	{
		auto data = bin_->get_data();
		while (index_ < bin_->size() && g_parser_info.char_table_[data[index_]] == token) {
			++index_;
		}
	}

	void feed_one_block(size_t begin)
	{
		eat(parser_info::BLOB);

		std::string name(bin_->get_data() + begin, index_ - begin);

		auto new_block = std::make_shared<base_res>(std::move(name));

		// pop the stack
		assert(indent_ <= stack_.size() - 1);
		while (stack_.size() > indent_ + 1) {
			stack_.pop_back();
		}

		(*stack_.rbegin())->add_child(name, new_block);
		stack_.push_back(std::move(new_block));
	}

private:
	binary_ptr bin_;
	res_ptr root_;
	size_t index_;
	size_t indent_;
	res_vector stack_;
};

res_ptr res::parse(binary_ptr&& bin)
{
	res_parser parser(bin);
	
	return parser.parse();
}