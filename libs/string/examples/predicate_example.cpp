//  Boost string_algo library example file  ---------------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <hpx/string/classification.hpp>
#include <hpx/string/predicate.hpp>

#include <functional>
#include <iostream>
#include <string>

int main()
{
    std::cout << "* Predicate Example *" << std::endl << std::endl;

    std::string str1("123xxx321");
    std::string str2("abc");

    // Check if str1 starts with '123'
    std::cout << "str1 starts with \"123\": "
              << (hpx::string::starts_with(str1, std::string("123")) ? "true" :
                                                                       "false")
              << std::endl;

    // Check if str1 ends with '123'
    std::cout << "str1 ends with \"123\": "
              << (hpx::string::ends_with(str1, std::string("123")) ? "true" :
                                                                     "false")
              << std::endl;

    // Check if str1 contains 'xxx'
    std::cout << "str1 contains \"xxx\": "
              << (hpx::string::contains(str1, std::string("xxx")) ? "true" :
                                                                    "false")
              << std::endl;

    // Check if str2 equals to 'abc'
    std::cout << "str2 equals \"abc\": "
              << (hpx::string::equals(str2, std::string("abc")) ? "true" :
                                                                  "false")
              << std::endl;

    // Classification functors and all predicate
    if (hpx::string::all(";.,", hpx::string::is_punct()))
    {
        std::cout << "\";.,\" are all punctuation characters" << std::endl;
    }

    // Classification predicates can be combined
    if (hpx::string::all("abcxxx",
            hpx::string::is_any_of("xabc") && !hpx::string::is_space()))
    {
        std::cout << "true" << std::endl;
    }

    std::cout << std::endl;

    return 0;
}
