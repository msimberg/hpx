//  Boost string_algo library iterator_test.cpp file  ---------------------------//

//  SPDX-License-Identifier: BSL-1.0
//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <hpx/string/classification.hpp>
#include <hpx/string/predicate.hpp>
#include <hpx/string/split.hpp>
#include <hpx/testing.hpp>

#include <iostream>
#include <list>
#include <string>
#include <vector>

using namespace hpx::string;

template <typename T1, typename T2>
void deep_compare(const T1& X, const T2& Y)
{
    HPX_TEST(X.size() == Y.size());
    for (unsigned int nIndex = 0; nIndex < X.size(); ++nIndex)
    {
        HPX_TEST(equals(X[nIndex], Y[nIndex]));
    }
}

void iterator_test()
{
    std::string str1("xx-abc--xx-abb");
    std::string str2("Xx-abc--xX-abb-xx");
    std::string str3("xx");
    std::string strempty("");
    const char* pch1 = "xx-abc--xx-abb";
    std::vector<std::string> tokens;
    std::vector<std::vector<int>> vtokens;

    // find_all tests
    find_all(tokens, pch1, "xx");

    HPX_TEST(tokens.size() == 2);
    HPX_TEST(tokens[0] == std::string("xx"));
    HPX_TEST(tokens[1] == std::string("xx"));

    ifind_all(tokens, str2, "xx");

    HPX_TEST(tokens.size() == 3);
    HPX_TEST(tokens[0] == std::string("Xx"));
    HPX_TEST(tokens[1] == std::string("xX"));
    HPX_TEST(tokens[2] == std::string("xx"));

    find_all(tokens, str1, "xx");

    HPX_TEST(tokens.size() == 2);
    HPX_TEST(tokens[0] == std::string("xx"));
    HPX_TEST(tokens[1] == std::string("xx"));

    find_all(vtokens, str1, std::string("xx"));
    deep_compare(tokens, vtokens);

    // If using a compiler that supports forwarding references, we should be able to use rvalues, too
    find_all(tokens, std::string("xx-abc--xx-abb"), "xx");

    HPX_TEST(tokens.size() == 2);
    HPX_TEST(tokens[0] == std::string("xx"));
    HPX_TEST(tokens[1] == std::string("xx"));

    ifind_all(tokens, std::string("Xx-abc--xX-abb-xx"), "xx");

    HPX_TEST(tokens.size() == 3);
    HPX_TEST(tokens[0] == std::string("Xx"));
    HPX_TEST(tokens[1] == std::string("xX"));
    HPX_TEST(tokens[2] == std::string("xx"));

    // split tests
    split(tokens, str2, is_any_of("xX"), token_compress_on);

    HPX_TEST(tokens.size() == 4);
    HPX_TEST(tokens[0] == std::string(""));
    HPX_TEST(tokens[1] == std::string("-abc--"));
    HPX_TEST(tokens[2] == std::string("-abb-"));
    HPX_TEST(tokens[3] == std::string(""));

    split(tokens, pch1, is_any_of("x"), token_compress_on);

    HPX_TEST(tokens.size() == 3);
    HPX_TEST(tokens[0] == std::string(""));
    HPX_TEST(tokens[1] == std::string("-abc--"));
    HPX_TEST(tokens[2] == std::string("-abb"));

    split(vtokens, str1, is_any_of("x"), token_compress_on);
    deep_compare(tokens, vtokens);

    split(tokens, str1, is_punct(), token_compress_off);

    HPX_TEST(tokens.size() == 5);
    HPX_TEST(tokens[0] == std::string("xx"));
    HPX_TEST(tokens[1] == std::string("abc"));
    HPX_TEST(tokens[2] == std::string(""));
    HPX_TEST(tokens[3] == std::string("xx"));
    HPX_TEST(tokens[4] == std::string("abb"));

    split(tokens, str3, is_any_of(","), token_compress_off);

    HPX_TEST(tokens.size() == 1);
    HPX_TEST(tokens[0] == std::string("xx"));

    split(tokens, strempty, is_punct(), token_compress_off);

    HPX_TEST(tokens.size() == 1);
    HPX_TEST(tokens[0] == std::string(""));

    // If using a compiler that supports forwarding references, we should be able to use rvalues, too
    split(tokens, std::string("Xx-abc--xX-abb-xx"), is_any_of("xX"),
        token_compress_on);

    HPX_TEST(tokens.size() == 4);
    HPX_TEST(tokens[0] == std::string(""));
    HPX_TEST(tokens[1] == std::string("-abc--"));
    HPX_TEST(tokens[2] == std::string("-abb-"));
    HPX_TEST(tokens[3] == std::string(""));

    find_iterator<std::string::iterator> fiter =
        make_find_iterator(str1, first_finder("xx"));
    find_iterator<std::string::iterator> fiter2;

    HPX_TEST(equals(*fiter, "xx"));
    ++fiter;

    fiter2 = fiter;
    HPX_TEST(equals(*fiter, "xx"));
    HPX_TEST(equals(*fiter2, "xx"));

    ++fiter;
    HPX_TEST(fiter == find_iterator<std::string::iterator>());
    HPX_TEST(equals(*fiter2, "xx"));

    ++fiter2;
    HPX_TEST(fiter2 == find_iterator<std::string::iterator>());

    split_iterator<std::string::iterator> siter = make_split_iterator(
        str1, token_finder(is_any_of("-"), token_compress_on));
    split_iterator<std::string::iterator> siter2;
    HPX_TEST(equals(*siter, "xx"));
    ++siter;

    siter2 = siter;
    HPX_TEST(equals(*siter, "abc"));
    HPX_TEST(equals(*siter2, "abc"));

    ++siter;
    HPX_TEST(equals(*siter, "xx"));
    HPX_TEST(equals(*siter2, "abc"));

    ++siter;
    HPX_TEST(equals(*siter, "abb"));
    ++siter;
    HPX_TEST(siter == split_iterator<std::string::iterator>(siter));
    HPX_TEST(siter == split_iterator<std::string::iterator>());

    //  Make sure we work with forward iterators
    //  See bug #7989
    std::list<char> l1;
    find_iterator<std::list<char>::iterator> liter =
        make_find_iterator(l1, first_finder("xx"));
}

int main()
{
    iterator_test();
}
