//  Boost string_algo library trim_test.cpp file  ---------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <hpx/string/trim.hpp>
#include <hpx/string/trim_all.hpp>
#include <hpx/testing.hpp>

#include <iostream>
#include <string>

using namespace hpx::string;

void trim_test()
{
    std::string str1("     1x x x x1     ");
    std::string str2("     2x x x x2     ");
    std::string str3("    ");

    // *** value passing tests *** //

    // general string test
    HPX_TEST(trim_left_copy(str1) == "1x x x x1     ");
    HPX_TEST(trim_right_copy(str1) == "     1x x x x1");
    HPX_TEST(trim_copy(str1) == "1x x x x1");

    // spaces-only string test
    HPX_TEST(trim_left_copy(str3) == "");
    HPX_TEST(trim_right_copy(str3) == "");
    HPX_TEST(trim_copy(str3) == "");

    // empty string check
    HPX_TEST(trim_left_copy(std::string("")) == "");
    HPX_TEST(trim_right_copy(std::string("")) == "");
    HPX_TEST(trim_copy(std::string("")) == "");

    // iterator tests
    std::string str;
    trim_left_copy_if(std::back_inserter(str), str1, is_space());
    HPX_TEST(str == "1x x x x1     ");

    str.clear();
    trim_right_copy_if(std::back_inserter(str), str1, is_space());
    HPX_TEST(str == "     1x x x x1");

    str.clear();
    trim_copy_if(std::back_inserter(str), str1, is_space());
    HPX_TEST(str == "1x x x x1");

    str.clear();
    trim_left_copy_if(
        std::back_inserter(str), "     1x x x x1     ", is_space());
    HPX_TEST(str == "1x x x x1     ");

    str.clear();
    trim_right_copy_if(
        std::back_inserter(str), "     1x x x x1     ", is_space());
    HPX_TEST(str == "     1x x x x1");

    str.clear();
    trim_copy_if(std::back_inserter(str), "     1x x x x1     ", is_space());
    HPX_TEST(str == "1x x x x1");
    // *** inplace tests *** //

    // general string test
    trim_left(str1);
    HPX_TEST(str1 == "1x x x x1     ");
    trim_right(str1);
    HPX_TEST(str1 == "1x x x x1");
    trim(str2);
    HPX_TEST(str2 == "2x x x x2");

    // spaces-only string test
    str3 = "    ";
    trim_left(str3);
    HPX_TEST(str3 == "");
    str3 = "    ";
    trim_right(str3);
    HPX_TEST(str3 == "");
    str3 = "    ";
    trim(str3);
    HPX_TEST(str3 == "");

    // empty string check
    str3 = "";
    trim_left(str3);
    HPX_TEST(str3 == "");
    str3 = "";
    trim_right(str3);
    HPX_TEST(str3 == "");
    str3 = "";
    trim(str3);
    HPX_TEST(str3 == "");

    // *** non-standard predicate tests *** //
    HPX_TEST(trim_copy_if(std::string("123abc456"),
                 is_classified(std::ctype_base::digit)) == "abc");
    HPX_TEST(trim_copy_if(std::string("<>abc<>"), is_any_of("<<>>")) == "abc");
}

void trim_all_test()
{
    std::string str1("     1x   x   x   x1     ");
    std::string str2("+---...2x+--x--+x-+-x2...---+");
    std::string str3("    ");

    // *** value passing tests *** //

    // general string test
    HPX_TEST(trim_all_copy(str1) == "1x x x x1");
    HPX_TEST(trim_all_copy_if(str2, is_punct()) == "2x+x-x-x2");

    // spaces-only string test
    HPX_TEST(trim_all_copy(str3) == "");

    // empty string check
    HPX_TEST(trim_all_copy(std::string("")) == "");

    // general string test
    trim_all(str1);
    HPX_TEST(str1 == "1x x x x1");
    trim_all_if(str2, is_punct());
    HPX_TEST(str2 == "2x+x-x-x2");

    // spaces-only string test
    str3 = "    ";
    trim_all(str3);
    HPX_TEST(str3 == "");

    // empty string check
    str3 = "";
    trim_all(str3);
    HPX_TEST(str3 == "");
    HPX_TEST(str3 == "");

    // *** non-standard predicate tests *** //
    HPX_TEST(trim_all_copy_if(std::string("123abc127deb456"),
                 is_classified(std::ctype_base::digit)) == "abc1deb");
    HPX_TEST(trim_all_copy_if(std::string("<>abc<>def<>"), is_any_of("<<>>")) ==
        "abc<def");
}

void trim_fill_test()
{
    std::string str1("     1x   x   x   x1     ");
    std::string str2("+---...2x+--x--+x-+-x2...---+");
    std::string str3("    ");

    // *** value passing tests *** //

    // general string test
    HPX_TEST(trim_fill_copy(str1, "-") == "1x-x-x-x1");
    HPX_TEST(trim_fill_copy_if(str2, " ", is_punct()) == "2x x x x2");

    // spaces-only string test
    HPX_TEST(trim_fill_copy(str3, " ") == "");

    // empty string check
    HPX_TEST(trim_fill_copy(std::string(""), " ") == "");

    // general string test
    trim_fill(str1, "-");
    HPX_TEST(str1 == "1x-x-x-x1");
    trim_fill_if(str2, "", is_punct());
    HPX_TEST(str2 == "2xxxx2");

    // spaces-only string test
    str3 = "    ";
    trim_fill(str3, "");
    HPX_TEST(str3 == "");

    // empty string check
    str3 = "";
    trim_fill(str3, "");
    HPX_TEST(str3 == "");
    HPX_TEST(str3 == "");

    // *** non-standard predicate tests *** //
    HPX_TEST(trim_fill_copy_if(std::string("123abc127deb456"), "+",
                 is_classified(std::ctype_base::digit)) == "abc+deb");
    HPX_TEST(trim_fill_copy_if(std::string("<>abc<>def<>"), "-",
                 is_any_of("<<>>")) == "abc-def");
}

int main()
{
    trim_test();
    trim_all_test();
    trim_fill_test();
}
