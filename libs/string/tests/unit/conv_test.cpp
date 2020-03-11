//  Boost string_algo library conv_test.cpp file  ---------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <hpx/string/case_conv.hpp>
#include <hpx/testing.hpp>

#include <algorithm>
#include <iostream>
#include <string>

using namespace hpx::string;

int main()
{
    std::string str1("AbCdEfG 123 xxxYYYzZzZ");
    std::string str2("AbCdEfG 123 xxxYYYzZzZ");
    std::string str3("");
    const char pch[] = "AbCdEfG 123 xxxYYYzZzZ";
    unsigned int pchlen = sizeof(pch);

    char* pch1 = new char[pchlen];
    std::copy(pch, pch + pchlen, pch1);
    char* pch2 = new char[pchlen];
    std::copy(pch, pch + pchlen, pch2);

    // *** iterator tests *** //

    std::string strout;
    to_lower_copy(back_inserter(strout), str1);
    HPX_TEST(strout == "abcdefg 123 xxxyyyzzzz");
    strout.clear();
    to_upper_copy(back_inserter(strout), str1);
    HPX_TEST(strout == "ABCDEFG 123 XXXYYYZZZZ");

    strout.clear();
    to_lower_copy(back_inserter(strout), "AbCdEfG 123 xxxYYYzZzZ");
    HPX_TEST(strout == "abcdefg 123 xxxyyyzzzz");
    strout.clear();
    to_upper_copy(back_inserter(strout), "AbCdEfG 123 xxxYYYzZzZ");
    HPX_TEST(strout == "ABCDEFG 123 XXXYYYZZZZ");

    strout.clear();
    to_lower_copy(back_inserter(strout), pch1);
    HPX_TEST(strout == "abcdefg 123 xxxyyyzzzz");
    strout.clear();
    to_upper_copy(back_inserter(strout), pch1);
    HPX_TEST(strout == "ABCDEFG 123 XXXYYYZZZZ");

    // *** value passing tests *** //

    HPX_TEST(to_lower_copy(str1) == "abcdefg 123 xxxyyyzzzz");
    HPX_TEST(to_upper_copy(str1) == "ABCDEFG 123 XXXYYYZZZZ");

    HPX_TEST(to_lower_copy(str3) == "");
    HPX_TEST(to_upper_copy(str3) == "");

    // *** inplace tests *** //

    to_lower(str1);
    HPX_TEST(str1 == "abcdefg 123 xxxyyyzzzz");
    to_upper(str2);
    HPX_TEST(str2 == "ABCDEFG 123 XXXYYYZZZZ");

    // c-string modification
    to_lower(pch1);
    HPX_TEST(std::string(pch1) == "abcdefg 123 xxxyyyzzzz");
    to_upper(pch2);
    HPX_TEST(std::string(pch2) == "ABCDEFG 123 XXXYYYZZZZ");

    to_lower(str3);
    HPX_TEST(str3 == "");
    to_upper(str3);
    HPX_TEST(str3 == "");

    delete[] pch1;
    delete[] pch2;
}
