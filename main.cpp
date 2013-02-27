/*=============================================================================
    Copyright (c) 2002-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A complex number micro parser.
//
//  [ JDG May 10, 2002 ]    spirit1
//  [ JDG May 9, 2007 ]     spirit2
//
///////////////////////////////////////////////////////////////////////////////

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


#define LOG_INFO(X) std::cout << X;
#define LOG_ERROR(X) std::cerr << X;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template <typename Iterator>
struct header_tokens : qi::grammar<Iterator, std::map<std::string, std::string>()> {
	// Content-Disposition: form-data; name="sha1-9b03f7aca1ac60d40b5e570c34f79a3e07c918e8"; filename="blob1"

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

		content = chunk >> *chunk >> *end_boundry;
		chunk = chunk_boundry >> value;
		chunk_boundry = qi::lit("--") >> boundry >> boost::spirit::eol;
		end_boundry = qi::lit("--") >> boundry >> qi::lit("--");
		value = +(ascii::char_ - chunk_boundry);
	}

	qi::rule<Iterator, std::vector<std::string>()> content;
	qi::rule<Iterator, std::string()> value, chunk, chunk_boundry, end_boundry;
};

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
			std::cout << " - value: "<< std::endl << *iter << std::endl << std::endl;
		}
		return;
	}
	LOG_INFO("Failure matching " << std::endl << std::endl << input << std::endl);
}

const char *input_1 = 
"--randomboundaryXYZ\n"
"Content-Disposition: form-data; name=\"sha1-9b03f7aca1ac60d40b5e570c34f79a3e07c918e8\"; filename=\"blob1\"\n"
"Content-Type: application/octet-stream\n\n"
"(binary or text blob data)\n"
"--randomboundaryXYZ\n"
"Content-Disposition: form-data; name=\"sha1-deadbeefdeadbeefdeadbeefdeadbeefdeadbeef\"; filename=\"blob2\"\n"
"Content-Type: application/octet-stream\n\n"
"(binary or text blob data)\n"
"--randomboundaryXYZ--\n";


int main() {
	test_header_tokens("form-data; name=\"sha1-9b03f7aca1ac60d40b5e570c34f79a3e07c918e8\"; filename=\"blob1\"");
	std::string input1(input_1, 0, strlen(input_1));
	test_chunks("randomboundaryXYZ", input1);
	return 0;
}