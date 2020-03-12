//  Boost string_algo library find_format_test.cpp file  ------------------//

//  SPDX-License-Identifier: BSL-1.0
//  Copyright (c) 2009 Steven Watanabe
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

//  hpxinspect:nodeprecatedname:boost::iterator_range

#include <hpx/string/find_format.hpp>
#include <hpx/string/finder.hpp>
#include <hpx/string/formatter.hpp>
#include <hpx/testing.hpp>

#include <string>

using namespace hpx::string;

// We're only using const_formatter.
template <class Formatter>
struct formatter_result
{
    typedef boost::iterator_range<const char*> type;
};

template <class Formatter>
struct checked_formatter
{
public:
    checked_formatter(const Formatter& formatter)
      : formatter_(formatter)
    {
    }
    template <typename T>
    typename formatter_result<Formatter>::type operator()(const T& s) const
    {
        HPX_TEST(!s.empty());
        return formatter_(s);
    }

private:
    Formatter formatter_;
};

template <class Formatter>
checked_formatter<Formatter> make_checked_formatter(const Formatter& formatter)
{
    return checked_formatter<Formatter>(formatter);
}

void find_format_test()
{
    const std::string source = "$replace $replace";
    std::string expected = "ok $replace";
    std::string output(80, '\0');

    std::string::iterator pos = hpx::string::find_format_copy(output.begin(),
        source, hpx::string::first_finder("$replace"),
        make_checked_formatter(hpx::string::const_formatter("ok")));
    HPX_TEST(pos == output.begin() + expected.size());
    output.erase(std::remove(output.begin(), output.end(), '\0'), output.end());
    HPX_TEST_EQ(output, expected);

    output = hpx::string::find_format_copy(source,
        hpx::string::first_finder("$replace"),
        make_checked_formatter(hpx::string::const_formatter("ok")));
    HPX_TEST_EQ(output, expected);

    // now try finding a string that doesn't exist
    output.resize(80);
    pos = hpx::string::find_format_copy(output.begin(), source,
        hpx::string::first_finder("$noreplace"),
        make_checked_formatter(hpx::string::const_formatter("bad")));
    HPX_TEST(pos == output.begin() + source.size());
    output.erase(std::remove(output.begin(), output.end(), '\0'), output.end());
    HPX_TEST_EQ(output, source);

    output = hpx::string::find_format_copy(source,
        hpx::string::first_finder("$noreplace"),
        make_checked_formatter(hpx::string::const_formatter("bad")));
    HPX_TEST_EQ(output, source);

    // in place version
    output = source;
    hpx::string::find_format(output, hpx::string::first_finder("$replace"),
        make_checked_formatter(hpx::string::const_formatter("ok")));
    HPX_TEST_EQ(output, expected);
    output = source;
    hpx::string::find_format(output, hpx::string::first_finder("$noreplace"),
        make_checked_formatter(hpx::string::const_formatter("bad")));
    HPX_TEST_EQ(output, source);
}

void find_format_all_test()
{
    const std::string source = "$replace $replace";
    std::string expected = "ok ok";
    std::string output(80, '\0');

    std::string::iterator pos = hpx::string::find_format_all_copy(
        output.begin(), source, hpx::string::first_finder("$replace"),
        hpx::string::const_formatter("ok"));
    HPX_TEST(pos == output.begin() + expected.size());
    output.erase(std::remove(output.begin(), output.end(), '\0'), output.end());
    HPX_TEST_EQ(output, expected);

    output = hpx::string::find_format_all_copy(source,
        hpx::string::first_finder("$replace"),
        make_checked_formatter(hpx::string::const_formatter("ok")));
    HPX_TEST_EQ(output, expected);

    // now try finding a string that doesn't exist
    output.resize(80);
    pos = hpx::string::find_format_all_copy(output.begin(), source,
        hpx::string::first_finder("$noreplace"),
        make_checked_formatter(hpx::string::const_formatter("bad")));
    HPX_TEST(pos == output.begin() + source.size());
    output.erase(std::remove(output.begin(), output.end(), '\0'), output.end());
    HPX_TEST_EQ(output, source);

    output = hpx::string::find_format_all_copy(source,
        hpx::string::first_finder("$noreplace"),
        make_checked_formatter(hpx::string::const_formatter("bad")));
    HPX_TEST_EQ(output, source);

    // in place version
    output = source;
    hpx::string::find_format_all(output, hpx::string::first_finder("$replace"),
        make_checked_formatter(hpx::string::const_formatter("ok")));
    HPX_TEST_EQ(output, expected);
    output = source;
    hpx::string::find_format_all(output,
        hpx::string::first_finder("$noreplace"),
        make_checked_formatter(hpx::string::const_formatter("bad")));
    HPX_TEST_EQ(output, source);
}

int main()
{
    find_format_test();
    find_format_all_test();
}
