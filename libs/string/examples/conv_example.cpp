//  Boost string_algo library example file  ---------------------------------//

//  SPDX-License-Identifier: BSL-1.0
//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <hpx/string/case_conv.hpp>

#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main()
{
    std::cout << "* Case Conversion Example *" << std::endl << std::endl;

    std::string str1("AbCdEfG");
    std::vector<char> vec1(str1.begin(), str1.end());

    // Convert vector of chars to lower case
    std::cout << "lower-cased copy of vec1: ";
    hpx::string::to_lower_copy(std::ostream_iterator<char>(std::cout), vec1);
    std::cout << std::endl;

    // Convert string str1 to upper case ( copy the input )
    std::cout << "upper-cased copy of str1: "
              << hpx::string::to_upper_copy(str1) << std::endl;

    // Inplace conversion
    hpx::string::to_lower(str1);
    std::cout << "lower-cased str1: " << str1 << std::endl;

    std::cout << std::endl;

    return 0;
}
