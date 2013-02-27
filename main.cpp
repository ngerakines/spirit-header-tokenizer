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

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <iostream>
#include <string>

#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <iostream>

#define LOG_INFO(X) std::cout << X;
#define LOG_ERROR(X) std::cerr << X;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

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


int main() {
	std::string input("form-data; name=\"sha1-9b03f7aca1ac60d40b5e570c34f79a3e07c918e8\"; filename=\"blob1\"");
	std::string::iterator begin = input.begin();
	std::string::iterator end = input.end();
	header_tokens<std::string::iterator> p;
	std::map<std::string, std::string> m;
	bool result = qi::parse(begin, end, p, m);

	if (result) {
		std::map<std::string, std::string>::iterator iter;

		for (iter = m.begin(); iter != m.end(); ++iter) {
			LOG_INFO("key '" << iter->first << "' value '" << iter->second << "'" << std::endl);
		}
	}

	return 0;
}