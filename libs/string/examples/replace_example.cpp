//  Boost string_algo library example file  ---------------------------------//

//  SPDX-License-Identifier: BSL-1.0
//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <hpx/string.hpp>

#include <iostream>
#include <iterator>
#include <string>

// uppercase formatter
/*
    Convert an input to upper case.
    Note, that this formatter can be used only on std::string inputs.
*/
inline std::string upcase_formatter(
    const boost::iterator_range<std::string::const_iterator>& Replace)
{
    std::string Temp(Replace.begin(), Replace.end());
    hpx::string::to_upper(Temp);
    return Temp;
}

int main()
{
    std::cout << "* Replace Example *" << std::endl << std::endl;

    std::string str1("abc___cde___efg");

    // Erase 6-9th characters from the string
    std::cout << "str1 without 6th to 9th character:"
              << hpx::string::erase_range_copy(str1,
                     boost::make_iterator_range(
                         str1.begin() + 6, str1.begin() + 9))
              << std::endl;

    // Replace 6-9th character with '+++'
    std::cout << "str1 with 6th to 9th character replaced with '+++': "
              << hpx::string::replace_range_copy(str1,
                     boost::make_iterator_range(
                         str1.begin() + 6, str1.begin() + 9),
                     "+++")
              << std::endl;

    std::cout << "str1 with 'cde' replaced with 'XYZ': ";

    // Replace first 'cde' with 'XYZ'. Modify the input
    hpx::string::replace_first_copy(
        std::ostream_iterator<char>(std::cout), str1, "cde", "XYZ");
    std::cout << std::endl;

    // Replace all '___'
    std::cout << "str1 with all '___' replaced with '---': "
              << hpx::string::replace_all_copy(str1, "___", "---") << std::endl;

    // Erase all '___'
    std::cout << "str1 without all '___': "
              << hpx::string::erase_all_copy(str1, "___") << std::endl;

    // replace third and 5th occurrence of _ in str1
    // note that nth argument is 0-based
    hpx::string::replace_nth(str1, "_", 4, "+");
    hpx::string::replace_nth(str1, "_", 2, "+");

    std::cout << "str1 with third and 5th occurrence of _ replace: " << str1
              << std::endl;

    // Custom formatter examples
    std::string str2("abC-xxxx-AbC-xxxx-abc");

    // Find string 'abc' ignoring the case and convert it to upper case
    std::cout << "Upcase all 'abc'(s) in the str2: "
              << hpx::string::find_format_all_copy(str2,
                     first_finder("abc", hpx::string::is_iequal()),
                     upcase_formatter);

    std::cout << std::endl;

    return 0;
}
