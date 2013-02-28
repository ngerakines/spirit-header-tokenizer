
#include "boost-multipart-form.h"

#include <boost/algorithm/string.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template <typename Iterator>
struct header_tokens : qi::grammar<Iterator, std::map<std::string, std::string>()> {

    header_tokens() : header_tokens::base_type(query) {
	query =  pair >> *(qi::lit(';') >> *(" ") >> pair) >> -(boost::spirit::eol);
	pair =  key >> -(qi::lit('=') >> value);
	key = +(ascii::char_ - qi::char_("=;"));
	value = (qi::lexeme['"' >> +(ascii::char_ - '"') >> '"'] | +(ascii::char_));
}

qi::rule<Iterator, std::map<std::string, std::string>()> query;
qi::rule<Iterator, std::pair<std::string, std::string>()> pair;
qi::rule<Iterator, std::string()> key, value;
};

template <typename Iterator>
struct chunks : qi::grammar<Iterator, std::vector<std::string>()> {

    chunks(std::string boundry) : chunks::base_type(content) {
	using qi::lit;
	using qi::lexeme;
	using qi::on_error;
	using qi::fail;
	using ascii::char_;
	using ascii::string;
	using namespace qi::labels;
	using phoenix::construct;
	using phoenix::val;
	content = chunk >> *chunk >> end_boundry;
	chunk = chunk_boundry >> value;
	chunk_boundry = qi::lit("--") >> boundry >> boost::spirit::eol;
	end_boundry = qi::lit("--") >> boundry >> qi::lit("--") >> -boost::spirit::eol;
	value = +(ascii::char_ - end_boundry - chunk_boundry);
}

qi::rule<Iterator, std::vector<std::string>()> content;
qi::rule<Iterator, std::string()> value, chunk, chunk_boundry, end_boundry;
};

template <typename Iterator>
struct chunk_tokens : qi::grammar<Iterator, std::vector<std::string>()> {

    chunk_tokens() : chunk_tokens::base_type(content) {
	using qi::lit;
	using qi::lexeme;
	using qi::on_error;
	using qi::fail;
	using ascii::char_;
	using ascii::string;
	using namespace qi::labels;
	using phoenix::construct;
	using phoenix::val;
	content = *header >> boost::spirit::eol >> *body;
	token = header | body;
	header = +(ascii::char_ - boost::spirit::eol) >> boost::spirit::eol;
	body = +(ascii::char_ | boost::spirit::eol);
}

qi::rule<Iterator, std::vector<std::string>()> content;
qi::rule<Iterator, std::string()> token, header, body;
};

/* void test_chunk_tokens(std::string input) {
	LOG_INFO(std::endl);
	std::string::iterator begin = input.begin();
	std::string::iterator end = input.end();
	chunk_tokens<std::string::iterator> p;
	std::vector<std::string> v;
	bool result = qi::parse(begin, end, p, v);

	if (result) {
		LOG_INFO("Success matching input" << std::endl);
		std::vector<std::string>::iterator iter;

		for (iter = v.begin(); iter != v.end(); ++iter) {
			std::cout << " - value: " << std::endl << *iter << std::endl << std::endl;
		}

		return;
	}

	LOG_INFO("Failure matching " << std::endl << std::endl << input << std::endl);
} */

Header::Header(std::string name, std::map<std::string, std::string> attributes) : name_(name), attributes_(attributes) {
}

std::string Header::attributeValue(std::string name) {

	std::string lower_name = boost::to_lower_copy(name);
	std::map<std::string, std::string>::iterator iter;


	for (iter = attributes_.begin(); iter!=attributes_.end(); ++iter) {
		if (boost::to_lower_copy((*iter).first) == lower_name) {
			return (*iter).second;
		}
	}

	return "";
}

void Part::add_header(std::string input) {

	unsigned split = input.find(":");
	if (split == std::string::npos) {
		LOG_ERROR("header not added: " << input << std::endl);
		return;
	}

	std::string header_name = input.substr(0, split);
	std::string header_value = input.substr(split + 1);

	header_tokens<std::string::iterator> header_tokens_parser;
	std::map<std::string, std::string> header_tokens_map;
	bool result = qi::parse(header_value.begin(), header_value.end(), header_tokens_parser, header_tokens_map);

	if (result == false) {
		LOG_ERROR("header not added: " << input << std::endl);
		return;
	}

	//std::map<std::string, std::string>::iterator iter;

	//for (iter = header_tokens_map.begin(); iter != header_tokens_map.end(); ++iter) {
		// LOG_INFO(" - key '" << iter->first << "' value '" << iter->second << "'" << std::endl);
	//}

	Header *header = new Header(header_name, header_tokens_map);
	add_header(header);
}

/*
void test_header_tokens(std::string input) {
	LOG_INFO(std::endl);
	std::string::iterator begin = input.begin();
	std::string::iterator end = input.end();
	header_tokens<std::string::iterator> p;
	std::map<std::string, std::string> m;
	bool result = qi::parse(begin, end, p, m);

	if (result) {
		LOG_INFO("Success matching " << input << std::endl);
		std::map<std::string, std::string>::iterator iter;

		for (iter = m.begin(); iter != m.end(); ++iter) {
			LOG_INFO(" - key '" << iter->first << "' value '" << iter->second << "'" << std::endl);
		}

		return;
	}

	LOG_INFO("Failure matching " << input << std::endl);
}

void test_chunks(std::string boundry, std::string input) {
	LOG_INFO(std::endl);
	std::string::iterator begin = input.begin();
	std::string::iterator end = input.end();
	chunks<std::string::iterator> p(boundry);
	std::vector<std::string> v;
	bool result = qi::parse(begin, end, p, v);

	if (result) {
		LOG_INFO("Success matching input" << std::endl);
		std::vector<std::string>::iterator iter;

		for (iter = v.begin(); iter != v.end(); ++iter) {
			std::cout << " - value: " << std::endl << *iter << std::endl << std::endl;
		}

		return;
	}

	LOG_INFO("Failure matching " << std::endl << std::endl << input << std::endl);
}
*/

const char *chunks_1 =
    "--randomboundaryXYZ\n"
    "Content-Disposition: form-data; name=\"sha1-9b03f7aca1ac60d40b5e570c34f79a3e07c918e8\"; filename=\"blob1\"\n"
    "Content-Type: application/octet-stream\n\n"
    "(binary or text blob data)\n"
    "--randomboundaryXYZ\n"
    "Content-Disposition: form-data; name=\"sha1-deadbeefdeadbeefdeadbeefdeadbeefdeadbeef\"; filename=\"blob2\"\n"
    "Content-Type: application/octet-stream\n\n"
    "(binary or text blob data)\n"
    "--randomboundaryXYZ--\n";

const char *chunk_1 =
    "Content-Disposition: form-data; name=\"sha1-deadbeefdeadbeefdeadbeefdeadbeefdeadbeef\"; filename=\"blob2\"\n"
    "Content-Type: application/octet-stream\n\n"
    "(binary or text blob data)\n"
    "(binary or text blob data)\n"
    "(binary or text blob data)\n";


MultiPartFormData* parse(std::string boundry, std::string input) {
	chunks<std::string::iterator> chunks_parser(boundry);
	std::vector<std::string> chunks_vector;
	bool result = qi::parse(input.begin(), input.end(), chunks_parser, chunks_vector);

	if (!result) {
		return NULL;
	}

	MultiPartFormData *multiPartFormData = new MultiPartFormData();
	multiPartFormData->boundry(boundry);
	std::vector<std::string>::iterator chunks_iter;

	for (chunks_iter = chunks_vector.begin(); chunks_iter != chunks_vector.end(); ++chunks_iter) {
		std::string chunk_data = *chunks_iter;
		chunk_tokens<std::string::iterator> chunk_parser;
		std::vector<std::string> chunk_vector;
		bool chunk_result = qi::parse(chunk_data.begin(), chunk_data.end(), chunk_parser, chunk_vector);

		if (chunk_result) {
			LOG_INFO(std::endl << "***** parsing part *****" << std::endl);
			Part *part = new Part();
			multiPartFormData->add_part(part);
			std::vector<std::string>::reverse_iterator chunk_iter;
			bool handled_last = false;

			for (chunk_iter = chunk_vector.rbegin(); chunk_iter != chunk_vector.rend(); ++chunk_iter) {
				// std::cout << " - value: " << std::endl << *chunk_iter << std::endl << std::endl;
				if (handled_last) {
					//LOG_INFO("part header " << *chunk_iter << std::endl);
					part->add_header(*chunk_iter);

				} else {
					//LOG_INFO("part payload " << *chunk_iter << std::endl);
					part->payload(*chunk_iter);
				}

				handled_last = true;
			}
			LOG_INFO("parse " << (*part) << std::endl);
		}
	}

	return multiPartFormData;
}

/* std::ostream& operator<<(std::ostream &strm, const MultiPartFormData &mpfd) {
  return strm << "MultiPartFormData(" << mpfd.boundry() << ")";
} */
std::ostream& operator<<(std::ostream& o, const MultiPartFormData& b) { return b.dump(o); }

std::ostream& operator<<(std::ostream& o, const Part& b) { return b.dump(o); }

std::ostream& operator<<(std::ostream& o, const Header& b) { return b.dump(o); }

int main() {
	/* test_header_tokens("form-data; name=\"sha1-9b03f7aca1ac60d40b5e570c34f79a3e07c918e8\"; filename=\"blob1\"");
	std::string chunks1(chunks_1, 0, strlen(chunks_1));
	test_chunks("randomboundaryXYZ", chunks1);
	test_chunk_tokens(chunk1); */
	std::string chunks1(chunks_1, 0, strlen(chunks_1));
	MultiPartFormData *mpfd = parse("randomboundaryXYZ", chunks1);

	LOG_INFO("parsed " << (*mpfd) << std::endl);

	return 0;
}