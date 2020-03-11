//  Boost string_algo library iterator_test.cpp file  ---------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <hpx/string/classification.hpp>
#include <hpx/string/join.hpp>
#include <hpx/string/predicate.hpp>
#include <hpx/testing.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace hpx::string;

bool is_not_empty(const std::string& str)
{
    return !str.empty();
}

void join_test()
{
    // Prepare inputs
    std::vector<std::string> tokens1;
    tokens1.push_back("xx");
    tokens1.push_back("abc");
    tokens1.push_back("xx");

    std::vector<std::string> tokens2;
    tokens2.push_back("");
    tokens2.push_back("xx");
    tokens2.push_back("abc");
    tokens2.push_back("");
    tokens2.push_back("abc");
    tokens2.push_back("xx");
    tokens2.push_back("");

    std::vector<std::string> tokens3;
    tokens3.push_back("");
    tokens3.push_back("");
    tokens3.push_back("");

    std::vector<std::string> empty_tokens;

    std::vector<std::vector<int>> vtokens;
    for (unsigned int n = 0; n < tokens2.size(); ++n)
    {
        vtokens.push_back(
            std::vector<int>(tokens2[n].begin(), tokens2[n].end()));
    }

    HPX_TEST(equals(join(tokens1, "-"), "xx-abc-xx"));
    HPX_TEST(equals(join(tokens2, "-"), "-xx-abc--abc-xx-"));
    HPX_TEST(equals(join(vtokens, "-"), "-xx-abc--abc-xx-"));
    HPX_TEST(equals(join(empty_tokens, "-"), ""));

    HPX_TEST(equals(join_if(tokens2, "-", is_not_empty), "xx-abc-abc-xx"));
    HPX_TEST(equals(join_if(empty_tokens, "-", is_not_empty), ""));
    HPX_TEST(equals(join_if(tokens3, "-", is_not_empty), ""));
}

int main()
{
    join_test();
}
