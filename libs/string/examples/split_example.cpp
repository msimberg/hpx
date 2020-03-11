//  Boost string_algo library example file  ---------------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <hpx/string/classification.hpp>
#include <hpx/string/find_iterator.hpp>
#include <hpx/string/split.hpp>

#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main()
{
    std::cout << "* Split Example *" << std::endl << std::endl;

    std::string str1("abc-*-ABC-*-aBc");

    std::cout << "Before: " << str1 << std::endl;

    // Find all 'abc' substrings (ignoring the case)
    // Create a find_iterator
    typedef hpx::string::find_iterator<std::string::iterator>
        string_find_iterator;
    for (string_find_iterator It = make_find_iterator(
             str1, first_finder("abc", hpx::string::is_iequal()));
         It != string_find_iterator(); ++It)
    {
        std::cout << boost::copy_range<std::string>(*It) << std::endl;
        // shift all chars in the match by one
        std::transform(It->begin(), It->end(), It->begin(),
            std::bind2nd(std::plus<char>(), 1));
    }

    // Print the string now
    std::cout << "After: " << str1 << std::endl;

    // Split the string into tokens ( use '-' and '*' as delimiters )
    // We need copies of the input only, and adjacent tokens are compressed
    std::vector<std::string> ResultCopy;
    split(ResultCopy, str1, hpx::string::is_any_of("-*"),
        hpx::string::token_compress_on);

    for (unsigned int nIndex = 0; nIndex < ResultCopy.size(); nIndex++)
    {
        std::cout << nIndex << ":" << ResultCopy[nIndex] << std::endl;
    };

    std::cout << std::endl;

    return 0;
}
