//  Boost string_algo library substr_test.cpp file  ------------------//

//  SPDX-License-Identifier: BSL-1.0
//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

//  hpxinspect:nodeprecatedname:boost::make_iterator_range

#include <hpx/string/classification.hpp>
#include <hpx/string/erase.hpp>
#include <hpx/string/finder.hpp>
#include <hpx/string/formatter.hpp>
#include <hpx/string/predicate.hpp>
#include <hpx/string/replace.hpp>
#include <hpx/string/std/list_traits.hpp>
#include <hpx/string/std/string_traits.hpp>
#include <hpx/testing.hpp>

#include <iostream>
#include <list>
#include <string>
#include <vector>

using namespace hpx::string;

void sequence_traits_test()
{
    // NOTE: We use !! to get an rvalue for HPX_TEST which takes a const&. Can't
    // take a reference of ::value directly.

    //  basic_string traits
    HPX_TEST(!!hpx::string::has_native_replace<std::string>::value);
    HPX_TEST(!hpx::string::has_stable_iterators<std::string>::value);
    HPX_TEST(!hpx::string::has_const_time_insert<std::string>::value);
    HPX_TEST(!hpx::string::has_const_time_erase<std::string>::value);

    // vector traits
    HPX_TEST(!hpx::string::has_native_replace<std::vector<char>>::value);
    HPX_TEST(!hpx::string::has_stable_iterators<std::vector<char>>::value);
    HPX_TEST(!hpx::string::has_const_time_insert<std::vector<char>>::value);
    HPX_TEST(!hpx::string::has_const_time_erase<std::vector<char>>::value);

    // list traits
    HPX_TEST(!hpx::string::has_native_replace<std::list<char>>::value);
    HPX_TEST(!!hpx::string::has_stable_iterators<std::list<char>>::value);
    HPX_TEST(!!hpx::string::has_const_time_insert<std::list<char>>::value);
    HPX_TEST(!!hpx::string::has_const_time_erase<std::list<char>>::value);
}

// Combine tests for all variants of the algorithm
#define C_ ,
#define TEST_ALGO(Algo, Input, Params, Output)                                 \
    {                                                                          \
        std::string str1(Input);                                               \
                                                                               \
        /* Copy test */                                                        \
        HPX_TEST(Algo##_copy(str1, Params) == Output);                         \
                                                                               \
        /* Iterator test */                                                    \
        std::string strout;                                                    \
        Algo##_copy(back_inserter(strout), str1, Params);                      \
        HPX_TEST(strout == Output);                                            \
                                                                               \
        /* In-place test */                                                    \
        std::vector<char> vec1(str1.begin(), str1.end());                      \
        std::list<char> list1(str1.begin(), str1.end());                       \
                                                                               \
        Algo(str1, Params);                                                    \
        HPX_TEST(equals(str1, Output));                                        \
                                                                               \
        Algo(vec1, Params);                                                    \
        HPX_TEST(equals(vec1, Output));                                        \
                                                                               \
        Algo(list1, Params);                                                   \
        HPX_TEST(equals(list1, Output));                                       \
    }

void replace_first_test()
{
    // replace first
    TEST_ALGO(replace_first, "1abc3abc2",
        std::string("abc") C_ std::string("YYY"), std::string("1YYY3abc2"));
    TEST_ALGO(
        ireplace_first, "1AbC3abc2", "aBc" C_ "YYY", std::string("1YYY3abc2"));
    TEST_ALGO(replace_first, "1abc3abc2",
        std::string("abc") C_ std::string("Z"), std::string("1Z3abc2"));
    TEST_ALGO(replace_first, "1abc3abc2",
        std::string("abc") C_ std::string("XXXX"), std::string("1XXXX3abc2"));
    TEST_ALGO(replace_first, "1abc3abc2",
        std::string("") C_ std::string("XXXX"), std::string("1abc3abc2"));
    TEST_ALGO(
        replace_first, "1abc3abc2", "" C_ "XXXX", std::string("1abc3abc2"));
    TEST_ALGO(replace_first, "", std::string("") C_ std::string("XXXX"),
        std::string(""));
    TEST_ALGO(
        erase_first, "1abc3abc2", std::string("abc"), std::string("13abc2"));
    TEST_ALGO(ierase_first, "1aBc3abc2", "abC", "13abc2");
    TEST_ALGO(erase_first, "1abc3abc2", "abc", "13abc2");
    TEST_ALGO(
        erase_first, "1abc3abc2", std::string(""), std::string("1abc3abc2"));
    TEST_ALGO(erase_first, "", std::string("abc"), std::string(""));
}

void replace_last_test()
{
    // replace last
    TEST_ALGO(replace_last, "1abc3abc2",
        std::string("abc") C_ std::string("YYY"), std::string("1abc3YYY2"));
    TEST_ALGO(
        ireplace_last, "1abc3AbC2", "aBc" C_ "YYY", std::string("1abc3YYY2"));
    TEST_ALGO(replace_last, "1abc3abc2", std::string("abc") C_ std::string("Z"),
        std::string("1abc3Z2"));
    TEST_ALGO(replace_last, "1abc3abc2",
        std::string("abc") C_ std::string("XXXX"), std::string("1abc3XXXX2"));
    TEST_ALGO(
        replace_last, "1abc3abc2", "abc" C_ "XXXX", std::string("1abc3XXXX2"));
    TEST_ALGO(replace_last, "", std::string("") C_ std::string("XXXX"),
        std::string(""));
    TEST_ALGO(
        erase_last, "1abc3abc2", std::string("abc"), std::string("1abc32"));
    TEST_ALGO(ierase_last, "1aBc3aBc2", "ABC", std::string("1aBc32"));
    TEST_ALGO(erase_last, "1abc3abc2", "abc", std::string("1abc32"));
    TEST_ALGO(
        erase_last, "1abc3abc2", std::string(""), std::string("1abc3abc2"));
    TEST_ALGO(erase_last, "", std::string("abc"), std::string(""));
}

void replace_all_test()
{
    // replace all
    TEST_ALGO(replace_all, "1abc3abc2",
        std::string("abc") C_ std::string("YYY"), std::string("1YYY3YYY2"));
    TEST_ALGO(replace_all, std::string("1abc3abc2"), "/" C_ "\\",
        std::string("1abc3abc2"));
    TEST_ALGO(
        ireplace_all, "1aBc3AbC2", "abC" C_ "YYY", std::string("1YYY3YYY2"));
    TEST_ALGO(replace_all, "1abc3abc2", std::string("abc") C_ std::string("Z"),
        std::string("1Z3Z2"));
    TEST_ALGO(replace_all, "1abc3abc2",
        std::string("abc") C_ std::string("XXXX"), std::string("1XXXX3XXXX2"));
    TEST_ALGO(
        replace_all, "1abc3abc2", "abc" C_ "XXXX", std::string("1XXXX3XXXX2"));
    TEST_ALGO(replace_all, "", std::string("") C_ std::string("XXXX"),
        std::string(""));
    TEST_ALGO(erase_all, "1abc3abc2", std::string("abc"), std::string("132"));
    TEST_ALGO(ierase_all, "1aBc3aBc2", "aBC", std::string("132"));
    TEST_ALGO(erase_all, "1abc3abc2", "abc", std::string("132"));
    TEST_ALGO(
        erase_all, "1abc3abc2", std::string(""), std::string("1abc3abc2"));
    TEST_ALGO(erase_all, "", std::string("abc"), std::string(""));
}

void replace_nth_test()
{
    // replace nth
    TEST_ALGO(replace_nth, "1abc3abc2",
        std::string("abc") C_ 0 C_ std::string("YYY"),
        std::string("1YYY3abc2"));
    TEST_ALGO(replace_nth, "1abc3abc2",
        std::string("abc") C_ - 1 C_ std::string("YYY"),
        std::string("1abc3YYY2"));
    TEST_ALGO(ireplace_nth, "1AbC3abc2", "aBc" C_ 0 C_ "YYY",
        std::string("1YYY3abc2"));
    TEST_ALGO(ireplace_nth, "1AbC3abc2", "aBc" C_ - 1 C_ "YYY",
        std::string("1AbC3YYY2"));
    TEST_ALGO(replace_nth, "1abc3abc2",
        std::string("abc") C_ 0 C_ std::string("Z"), std::string("1Z3abc2"));
    TEST_ALGO(replace_nth, "1abc3abc2",
        std::string("abc") C_ 0 C_ std::string("XXXX"),
        std::string("1XXXX3abc2"));
    TEST_ALGO(replace_nth, "1abc3abc2", "abc" C_ 0 C_ "XXXX",
        std::string("1XXXX3abc2"));
    TEST_ALGO(replace_nth, "1abc3abc2", "abc" C_ 3 C_ "XXXX",
        std::string("1abc3abc2"));
    TEST_ALGO(replace_nth, "1abc3abc2", "abc" C_ - 3 C_ "XXXX",
        std::string("1abc3abc2"));
    TEST_ALGO(replace_nth, "1abc3abc2",
        std::string("") C_ 0 C_ std::string("XXXX"), std::string("1abc3abc2"));
    TEST_ALGO(replace_nth, "", std::string("") C_ 0 C_ std::string("XXXX"),
        std::string(""));
    TEST_ALGO(replace_nth, "", std::string("") C_ - 1 C_ std::string("XXXX"),
        std::string(""));
    TEST_ALGO(
        erase_nth, "1abc3abc2", std::string("abc") C_ 0, std::string("13abc2"));
    TEST_ALGO(erase_nth, "1abc3abc2", std::string("abc") C_ - 1,
        std::string("1abc32"));
    TEST_ALGO(erase_nth, "1abc3abc2", std::string("abc") C_ - 3,
        std::string("1abc3abc2"));
    TEST_ALGO(ierase_nth, "1aBc3aBc2", "ABC" C_ 0, std::string("13aBc2"));
    TEST_ALGO(ierase_nth, "1aBc3aBc2", "ABC" C_ - 1, std::string("1aBc32"));
    TEST_ALGO(ierase_nth, "1aBc3aBc2", "ABC" C_ - 3, std::string("1aBc3aBc2"));
    TEST_ALGO(erase_nth, "1abc3abc2", "abc" C_ 0, std::string("13abc2"));
    TEST_ALGO(
        erase_nth, "1abc3abc2", std::string("") C_ 0, std::string("1abc3abc2"));
    TEST_ALGO(erase_nth, "", std::string("abc") C_ 0, std::string(""));
    TEST_ALGO(erase_nth, "", std::string("abc") C_ - 1, std::string(""));
    TEST_ALGO(replace_nth, "1abc3abc2",
        std::string("abc") C_ 1 C_ std::string("YYY"),
        std::string("1abc3YYY2"));
    TEST_ALGO(replace_nth, "1abc3abc2",
        std::string("abc") C_ 2 C_ std::string("YYY"),
        std::string("1abc3abc2"));
}

void replace_head_test()
{
    // replace head
    TEST_ALGO(replace_head, "abc3abc2", 3 C_ std::string("YYY"),
        std::string("YYY3abc2"));
    TEST_ALGO(replace_head, "abc3abc2", -3 C_ std::string("YYY"),
        std::string("YYYbc2"));
    TEST_ALGO(replace_head, "abc3abc2", 3 C_ "YYY", std::string("YYY3abc2"));
    TEST_ALGO(replace_head, "abc", 3 C_ std::string("Z"), std::string("Z"));
    TEST_ALGO(
        replace_head, "abc", 6 C_ std::string("XXXX"), std::string("XXXX"));
    TEST_ALGO(
        replace_head, "abc", -6 C_ std::string("XXXX"), std::string("abc"));
    TEST_ALGO(replace_head, "abc3abc2", 0 C_ std::string("XXXX"),
        std::string("abc3abc2"));
    TEST_ALGO(replace_head, "", 4 C_ std::string("XXXX"), std::string(""));
    TEST_ALGO(replace_head, "", -4 C_ std::string("XXXX"), std::string(""));
    TEST_ALGO(erase_head, "abc3abc2", 3, std::string("3abc2"));
    TEST_ALGO(erase_head, "abc3abc2", -3, std::string("bc2"));
    TEST_ALGO(erase_head, "abc3abc2", 0, std::string("abc3abc2"));
    TEST_ALGO(erase_head, "", 4, std::string(""));
    TEST_ALGO(erase_head, "", -4, std::string(""));
}

void replace_tail_test()
{
    // replace tail
    TEST_ALGO(replace_tail, "abc3abc", 3 C_ std::string("YYY"),
        std::string("abc3YYY"));
    TEST_ALGO(replace_tail, "abc3abc", -3 C_ "YYY", std::string("abcYYY"));
    TEST_ALGO(replace_tail, "abc", 3 C_ std::string("Z"), std::string("Z"));
    TEST_ALGO(
        replace_tail, "abc", 6 C_ std::string("XXXX"), std::string("XXXX"));
    TEST_ALGO(
        replace_tail, "abc", -6 C_ std::string("XXXX"), std::string("abc"));
    TEST_ALGO(replace_tail, "abc3abc", 0 C_ std::string("XXXX"),
        std::string("abc3abc"));
    TEST_ALGO(replace_tail, "", 4 C_ std::string("XXXX"), std::string(""));
    TEST_ALGO(replace_tail, "", -4 C_ std::string("XXXX"), std::string(""));
    TEST_ALGO(erase_tail, "abc3abc", 3, std::string("abc3"));
    TEST_ALGO(erase_tail, "abc3abc", -3, std::string("abc"));
    TEST_ALGO(erase_tail, "abc3abc", 0, std::string("abc3abc"));
    TEST_ALGO(erase_tail, "", 4, std::string(""));
    TEST_ALGO(erase_tail, "", -4, std::string(""));
}

void replace_range_test()
{
    // replace_range
    {
        std::string str1("1abc3abc2");
        HPX_TEST(
            replace_range_copy(str1,
                boost::make_iterator_range(str1.begin() + 1, str1.begin() + 4),
                std::string("XXX")) == std::string("1XXX3abc2"));

        std::string strout;
        replace_range_copy(back_inserter(strout), str1,
            boost::make_iterator_range(str1.begin() + 1, str1.begin() + 4),
            std::string("XXX"));
        HPX_TEST(strout == std::string("1XXX3abc2"));

        replace_range(str1,
            boost::make_iterator_range(str1.begin() + 1, str1.begin() + 4),
            std::string("XXX"));
        HPX_TEST(str1 == std::string("1XXX3abc2"));
    }
    // erase_range
    {
        std::string str1("1abc3abc2");
        HPX_TEST(erase_range_copy(str1,
                     boost::make_iterator_range(str1.begin() + 1,
                         str1.begin() + 4)) == std::string("13abc2"));

        std::string strout;
        erase_range_copy(back_inserter(strout), str1,
            boost::make_iterator_range(str1.begin() + 1, str1.begin() + 4));
        HPX_TEST(strout == std::string("13abc2"));

        erase_range(str1,
            boost::make_iterator_range(str1.begin() + 1, str1.begin() + 4));
        HPX_TEST(str1 == std::string("13abc2"));
    }
}

void collection_comp_test()
{
    // container traits compatibility tests
    {
        std::string strout;
        replace_first_copy(back_inserter(strout), "1abc3abc2", "abc", "YYY");
        HPX_TEST(strout == std::string("1YYY3abc2"));
    }
    {
        std::string strout;
        replace_last_copy(back_inserter(strout), "1abc3abc2", "abc", "YYY");
        HPX_TEST(strout == std::string("1abc3YYY2"));
    }
    {
        std::string strout;
        replace_all_copy(back_inserter(strout), "1abc3abc2", "abc", "YYY");
        HPX_TEST(strout == std::string("1YYY3YYY2"));
    }
    {
        std::string strout;
        replace_nth_copy(back_inserter(strout), "1abc3abc2", "abc", 1, "YYY");
        HPX_TEST(strout == std::string("1abc3YYY2"));
    }
    {
        std::string strout;
        replace_head_copy(back_inserter(strout), "abc3abc2", 3, "YYY");
        HPX_TEST(strout == std::string("YYY3abc2"));
    }
    {
        std::string strout;
        replace_tail_copy(back_inserter(strout), "abc3abc", 3, "YYY");
        HPX_TEST(strout == std::string("abc3YYY"));
    }
}

void dissect_format_test()
{
    HPX_TEST(find_format_all_copy(std::string("aBc123Abc"),
                 first_finder("abc", is_iequal()),
                 dissect_formatter(token_finder(is_upper()))) == "B123A");

    HPX_TEST(find_format_all_copy(std::string("abc   123   abc"),
                 token_finder(is_space(), token_compress_on),
                 dissect_formatter(head_finder(1))) == "abc 123 abc");
}

int main()
{
    sequence_traits_test();
    replace_first_test();
    replace_last_test();
    replace_all_test();
    replace_nth_test();
    replace_head_test();
    replace_tail_test();
    replace_range_test();
    collection_comp_test();
    dissect_format_test();
}
