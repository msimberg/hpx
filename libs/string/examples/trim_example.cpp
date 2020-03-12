//  Boost string_algo library example file  ---------------------------------//

//  SPDX-License-Identifier: BSL-1.0
//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <hpx/string/classification.hpp>
#include <hpx/string/trim.hpp>

#include <iostream>
#include <string>

int main()
{
    std::cout << "* Trim Example *" << std::endl << std::endl;

    std::string str1("     1x x x x1     ");
    std::string str2("<>trim<>");
    std::string str3("123abs343");

    // Simple left trim
    std::cout << "trim_left copy of str1: "
              << "\"" << hpx::string::trim_left_copy(str1) << "\"" << std::endl;

    // Inplace right trim
    hpx::string::trim_right(str1);
    std::cout << "trim_right on str1: "
              << "\"" << str1 << "\"" << std::endl;

    // Parametric trim. 'Space' is defined using is_any_of predicate
    std::cout << "trimmed copy of str4 ( space='<>' ): "
              << "\""
              << hpx::string::trim_copy_if(str2, hpx::string::is_any_of("<>"))
              << "\"" << std::endl;

    // Parametric trim. 'Space' is defined using is_digit predicate
    std::cout << "trimmed copy of str5 ( space=digit ): "
              << "\""
              << hpx::string::trim_copy_if(str3, hpx::string::is_digit())
              << "\"" << std::endl;

    std::cout << std::endl;

    return 0;
}
