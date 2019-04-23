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
		LIST_BEGIN,
		LIST_END
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
		char_table_['['] = LIST_BEGIN;
		char_table_[']'] = LIST_END;
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

			auto token = g_parser_info.char_table_[bin_->get_data()[index_]];

			if (token == parser_info::BLOB) {
				feed_one_block();
			}
			else if (token == parser_info::COMMENT) {
				skip_to(parser_info::NEW_LINE);
			}
			else if (token == parser_info::ASSIGN) {
				feed_assign();
			}
			else {
				++index_;
			}

			// reset the indent
			if (token == parser_info::INDENT) {
				++indent_;
			}
			else if (token == parser_info::NEW_LINE){
				indent_ = 0;
			}

//			++index_;
		}
	}

	void skip_to(parser_info::token_type token)
	{
		auto data = bin_->get_data();
		while (index_ < bin_->size() && g_parser_info.char_table_[data[index_]] != token) {
			++index_;
		}
	}

	void skip_space()
	{
		auto data = bin_->get_data();
		while (index_ < bin_->size()) {
			auto token = g_parser_info.char_table_[data[index_]];
			if (token == parser_info::SPACE) {
				++index_;
			}
			else {
				break;
			}
		}
	}

	void eat(parser_info::token_type token)
	{
		auto data = bin_->get_data();
		while (index_ < bin_->size() && g_parser_info.char_table_[data[index_]] == token) {
			++index_;
		}
	}

	void skip_to_str_end(char quote)
	{
		auto data = bin_->get_data();
		while (index_ < bin_->size()) {
			auto current = data[index_];
			if (current == quote) {
				break;
			}
			++index_;
		}
	}

	void scan_type_str()
	{
		auto data = bin_->get_data();
		int match = 1;
		while (index_ < bin_->size()) {
			auto token = g_parser_info.char_table_[data[index_]];
			if (token == parser_info::TYPE_START) {
				++match;
			}
			else if (token == parser_info::TYPE_END) {
				--match;
			}

			if (match == 0) {
				break;
			}

			++index_;
		}
	}

	std::string parse_str(char sep1 = '\0', char sep2 = '\0')
	{
		skip_space();

		auto data = bin_->get_data();
		auto begin = index_;
		auto end_char = index_;
		while (index_ < bin_->size()) {
			auto token = g_parser_info.char_table_[data[index_]];

			if (token == parser_info::NEW_LINE || 
				token == parser_info::COMMENT ||
				token == parser_info::TYPE_START ||
				token== parser_info::ASSIGN ||
				data[index_] == sep1 ||
				data[index_] == sep2) {
				break;
			}

			if (token == parser_info::STR_MARK) {
				auto quote = data[index_];
				++index_; // eat the ' or "
				begin = index_;
				// parse the string!
				skip_to_str_end(quote);
				end_char = index_;
				++index_; // erase the end ' or "
				break;
			}
			
			if (token != parser_info::SPACE) {
				end_char = index_ + 1;
			}

			++index_;
		}

		return std::string(data + begin, end_char - begin);
	}

	void feed_one_block()
	{
		std::string name = parse_str();

		auto new_block = std::make_shared<base_res>(std::move(name));

		// pop the stack by ident
		assert(indent_ <= stack_.size() - 1);
		while (stack_.size() > indent_ + 1) {
			stack_.pop_back();
		}

		(*stack_.rbegin())->add_child(new_block);
		stack_.push_back(std::move(new_block));

		// scan_type
		auto data = bin_->get_data();
		if (index_ < bin_->size()) {
			auto token = g_parser_info.char_table_[data[index_]];
			if (token == parser_info::TYPE_START) {
				++index_; // eat the {
				auto start = index_;
				scan_type_str();

				auto end = index_ - 1; // index include the last }
				++index_; // eat the }

				std::string type_str(data + start, end - start);
			}
		}
	}

	void feed_assign()
	{
		++index_; // eat the '='

		skip_space();
		if (index_ >= bin_->size()) {
			return;
		}

		auto token = g_parser_info.char_table_[bin_->get_data()[index_]];
		if (token == parser_info::LIST_BEGIN) {
			// parse the list
			++index_; // eat the [
			do {
				std::string value = parse_str(',', ']');
				(*stack_.rbegin())->add_child(std::make_shared<base_res>(value));;

				token = g_parser_info.char_table_[bin_->get_data()[index_]];
				++index_; // eat the , or ]
			} while (token != parser_info::LIST_END);
		}
		else {
			(*stack_.rbegin())->set_value(parse_str());
		}
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