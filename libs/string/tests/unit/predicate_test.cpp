//  Boost string_algo library predicate_test.cpp file  ------------------//

//  SPDX-License-Identifier: BSL-1.0
//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <hpx/string/classification.hpp>
#include <hpx/string/predicate.hpp>
#include <hpx/testing.hpp>

#include <functional>
#include <iostream>
#include <string>
#include <vector>

using namespace hpx::string;

void predicate_test()
{
    std::string str1("123xxx321");
    std::string str1_prefix("123");
    std::string str2("abc");
    std::string str3("");
    std::string str4("abc");
    std::vector<int> vec1(str1.begin(), str1.end());

    // Basic tests
    HPX_TEST(starts_with(str1, std::string("123")));
    HPX_TEST(!starts_with(str1, std::string("1234")));

    HPX_TEST(istarts_with("aBCxxx", "abc"));
    HPX_TEST(!istarts_with("aBCxxx", "abcd"));

    HPX_TEST(ends_with(str1, std::string("321")));
    HPX_TEST(!ends_with(str1, std::string("123")));

    HPX_TEST(iends_with("aBCxXx", "XXX"));
    HPX_TEST(!iends_with("aBCxxX", "xXXX"));

    HPX_TEST(contains(str1, std::string("xxx")));
    HPX_TEST(!contains(str1, std::string("yyy")));

    HPX_TEST(icontains("123XxX321", "xxx"));
    HPX_TEST(!icontains("123xXx321", "yyy"));

    HPX_TEST(equals(str2, std::string("abc")));
    HPX_TEST(!equals(str1, std::string("yyy")));

    HPX_TEST(iequals("AbC", "abc"));
    HPX_TEST(!iequals("aBc", "yyy"));

    HPX_TEST(lexicographical_compare("abc", "abd"));
    HPX_TEST(!lexicographical_compare("abc", "abc"));
    HPX_TEST(lexicographical_compare("abc", "abd", is_less()));

    HPX_TEST(!ilexicographical_compare("aBD", "AbC"));
    HPX_TEST(ilexicographical_compare("aBc", "AbD"));
    HPX_TEST(lexicographical_compare("abC", "aBd", is_iless()));

    // multi-type comparison test
    HPX_TEST(starts_with(vec1, std::string("123")));
    HPX_TEST(ends_with(vec1, std::string("321")));
    HPX_TEST(contains(vec1, std::string("xxx")));
    HPX_TEST(equals(vec1, str1));

    // overflow test
    HPX_TEST(!starts_with(str2, std::string("abcd")));
    HPX_TEST(!ends_with(str2, std::string("abcd")));
    HPX_TEST(!contains(str2, std::string("abcd")));
    HPX_TEST(!equals(str2, std::string("abcd")));

    // equal test
    HPX_TEST(starts_with(str2, std::string("abc")));
    HPX_TEST(ends_with(str2, std::string("abc")));
    HPX_TEST(contains(str2, std::string("abc")));
    HPX_TEST(equals(str2, std::string("abc")));

    //! Empty string test
    HPX_TEST(starts_with(str2, std::string("")));
    HPX_TEST(ends_with(str2, std::string("")));
    HPX_TEST(contains(str2, std::string("")));
    HPX_TEST(equals(str3, std::string("")));

    //! Container compatibility test
    HPX_TEST(starts_with("123xxx321", "123"));
    HPX_TEST(ends_with("123xxx321", "321"));
    HPX_TEST(contains("123xxx321", "xxx"));
    HPX_TEST(equals("123xxx321", "123xxx321"));
}

template <typename Pred, typename Input>
void test_pred(const Pred& pred, const Input& input, bool bYes)
{
    // test assignment operator
    Pred pred1 = pred;
    pred1 = pred;
    pred1 = pred1;
    if (bYes)
    {
        HPX_TEST(all(input, pred));
        HPX_TEST(all(input, pred1));
    }
    else
    {
        HPX_TEST(!all(input, pred));
        HPX_TEST(!all(input, pred1));
    }
}

#define TEST_CLASS(Pred, YesInput, NoInput)                                    \
    {                                                                          \
        test_pred(Pred, YesInput, true);                                       \
        test_pred(Pred, NoInput, false);                                       \
    }

void classification_test()
{
    TEST_CLASS(is_space(), "\n\r\t ", "...");
    TEST_CLASS(is_alnum(), "ab129ABc", "_ab129ABc");
    TEST_CLASS(is_alpha(), "abc", "abc1");
    TEST_CLASS(is_cntrl(), "\n\t\r", "...");
    TEST_CLASS(is_digit(), "1234567890", "abc");
    TEST_CLASS(is_graph(), "123abc.,", "  \t");
    TEST_CLASS(is_lower(), "abc", "Aasdf");
    TEST_CLASS(is_print(), "abs", "\003\004asdf");
    TEST_CLASS(is_punct(), ".,;\"", "abc");
    TEST_CLASS(is_upper(), "ABC", "aBc");
    TEST_CLASS(is_xdigit(), "ABC123", "XFD");
    TEST_CLASS(is_any_of(std::string("abc")), "aaabbcc", "aaxb");
    TEST_CLASS(is_any_of("abc"), "aaabbcc", "aaxb");
    TEST_CLASS(is_from_range('a', 'c'), "aaabbcc", "aaxb");

    TEST_CLASS(!is_classified(std::ctype_base::space), "...", "..\n\r\t ");
    TEST_CLASS((!is_any_of("abc") && is_from_range('a', 'e')) || is_space(),
        "d e", "abcde");

    // is_any_of test
    //  TEST_CLASS( !is_any_of(""), "", "aaa" )
    TEST_CLASS(is_any_of("a"), "a", "ab")
    TEST_CLASS(is_any_of("ba"), "ab", "abc")
    TEST_CLASS(is_any_of("cba"), "abc", "abcd")
    TEST_CLASS(is_any_of("hgfedcba"), "abcdefgh", "abcdefghi")
    TEST_CLASS(is_any_of("qponmlkjihgfedcba"), "abcdefghijklmnopq", "zzz")
}

#undef TEST_CLASS

int main()
{
    predicate_test();
    classification_test();
}
