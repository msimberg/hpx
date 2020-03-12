//  Boost string_algo library example file  ---------------------------------//

//  SPDX-License-Identifier: BSL-1.0
//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <hpx/string/case_conv.hpp>
#include <hpx/string/find.hpp>

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>

int main()
{
    std::cout << "* Find Example *" << std::endl << std::endl;

    std::string str1("abc___cde___efg");
    std::string str2("abc");

    // find "cde" substring
    boost::iterator_range<std::string::iterator> range =
        hpx::string::find_first(str1, std::string("cde"));

    // convert a substring to upper case
    // note that iterator range can be directly passed to the algorithm
    hpx::string::to_upper(range);

    std::cout << "str1 with upper-cased part matching cde: " << str1
              << std::endl;

    // get a head of the string
    boost::iterator_range<std::string::iterator> head =
        hpx::string::find_head(str1, 3);
    std::cout << "head(3) of the str1: "
              << std::string(head.begin(), head.end()) << std::endl;

    // get the tail
    head = hpx::string::find_tail(str2, 5);
    std::cout << "tail(5) of the str2: "
              << std::string(head.begin(), head.end()) << std::endl;

    // char processing
    char text[] = "hello dolly!";
    boost::iterator_range<char*> crange = hpx::string::find_last(text, "ll");

    // transform the range ( add 1 )
    std::transform(crange.begin(), crange.end(), crange.begin(),
        std::bind2nd(std::plus<char>(), 1));
    // uppercase the range
    hpx::string::to_upper(crange);

    std::cout << text << std::endl;

    std::cout << std::endl;

    return 0;
}
