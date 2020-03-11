//  Boost string_algo library substr_test.cpp file  ------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <hpx/testing.hpp>
#include <hpx/string/find.hpp>
#include <hpx/string/classification.hpp>
#include <hpx/string/split.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <sstream>

using namespace hpx::string;

int main()
{
    std::string str1("123abcxXxabcXxXabc321");
    std::string str2("abc");
    std::string str3("");
    const char* pch1="123abcxxxabcXXXabc321";
    std::vector<int> vec1( str1.begin(), str1.end() );

    // find results ------------------------------------------------------------//
    boost::iterator_range<std::string::iterator> nc_result;
    boost::iterator_range<std::string::const_iterator> cv_result;

    boost::iterator_range<std::vector<int>::iterator> nc_vresult;
    boost::iterator_range<std::vector<int>::const_iterator> cv_vresult;

    boost::iterator_range<const char*> ch_result;

    // basic tests ------------------------------------------------------------//


    // find_first
    nc_result=find_first( str1, std::string("abc") );
    HPX_TEST(
        ( (nc_result.begin()-str1.begin()) == 3) &&
        ( (nc_result.end()-str1.begin()) == 6) );

    cv_result=find_first( const_cast<const std::string&>(str1), str2 );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 3) &&
        ( (cv_result.end()-str1.begin()) == 6) );

    cv_result=ifind_first( const_cast<const std::string&>(str1), "xXX" );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 6) &&
        ( (cv_result.end()-str1.begin()) == 9) );

    ch_result=find_first( pch1, "abc" );
    HPX_TEST(( (ch_result.begin() - pch1 ) == 3) && ( (ch_result.end() - pch1 ) == 6 ) );

    // find_last
    nc_result=find_last( str1, std::string("abc") );
    HPX_TEST(
        ( (nc_result.begin()-str1.begin()) == 15) &&
        ( (nc_result.end()-str1.begin()) == 18) );

    cv_result=find_last( const_cast<const std::string&>(str1), str2 );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 15) &&
        ( (cv_result.end()-str1.begin()) == 18) );

    cv_result=ifind_last( const_cast<const std::string&>(str1), "XXx" );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 12) &&
        ( (cv_result.end()-str1.begin()) == 15) );

    ch_result=find_last( pch1, "abc" );
    HPX_TEST(( (ch_result.begin() - pch1 ) == 15) && ( (ch_result.end() - pch1 ) == 18 ) );

    // find_nth
    nc_result=find_nth( str1, std::string("abc"), 1 );
    HPX_TEST(
        ( (nc_result.begin()-str1.begin()) == 9) &&
        ( (nc_result.end()-str1.begin()) == 12) );

    nc_result=find_nth( str1, std::string("abc"), -1 );
    HPX_TEST(
        ( (nc_result.begin()-str1.begin()) == 15) &&
        ( (nc_result.end()-str1.begin()) == 18) );


    cv_result=find_nth( const_cast<const std::string&>(str1), str2, 1 );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 9) &&
        ( (cv_result.end()-str1.begin()) == 12) );

    cv_result=find_nth( const_cast<const std::string&>(str1), str2, -1 );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 15) &&
        ( (cv_result.end()-str1.begin()) == 18) );

    cv_result=ifind_nth( const_cast<const std::string&>(str1), "xxx", 1 );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 12) &&
        ( (cv_result.end()-str1.begin()) == 15) );

    cv_result=ifind_nth( const_cast<const std::string&>(str1), "xxx", 1 );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 12) &&
        ( (cv_result.end()-str1.begin()) == 15) );


    ch_result=find_nth( pch1, "abc", 1 );
    HPX_TEST(( (ch_result.begin() - pch1 ) == 9) && ( (ch_result.end() - pch1 ) == 12 ) );

    // find_head
    nc_result=find_head( str1, 6 );
    HPX_TEST(
        ( (nc_result.begin()-str1.begin()) == 0) &&
        ( (nc_result.end()-str1.begin()) == 6) );

    nc_result=find_head( str1, -6 );
    HPX_TEST(
        ( (nc_result.begin()-str1.begin()) == 0) &&
        ( (str1.end()-nc_result.end()) == 6 ) );

    cv_result=find_head( const_cast<const std::string&>(str1), 6 );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 0) &&
        ( (cv_result.end()-str1.begin()) == 6) );

    ch_result=find_head( pch1, 6 );
    HPX_TEST( ( (ch_result.begin() - pch1 ) == 0 ) && ( (ch_result.end() - pch1 ) == 6 ) );

    // find_tail
    nc_result=find_tail( str1, 6 );
    HPX_TEST(
        ( (nc_result.begin()-str1.begin()) == 15) &&
        ( (nc_result.end()-str1.begin()) == 21) );

    nc_result=find_tail( str1, -6 );
    HPX_TEST(
        ( (nc_result.begin()-str1.begin()) == 6) &&
        ( (nc_result.end()-str1.begin()) == 21) );


    cv_result=find_tail( const_cast<const std::string&>(str1), 6 );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 15) &&
        ( (cv_result.end()-str1.begin()) == 21) );

    ch_result=find_tail( pch1, 6 );
    HPX_TEST( ( (ch_result.begin() - pch1 ) == 15 ) && ( (ch_result.end() - pch1 ) == 21 ) );

    // find_token
    nc_result=find_token( str1, is_any_of("abc"), token_compress_on );
    HPX_TEST(
        ( (nc_result.begin()-str1.begin()) == 3) &&
        ( (nc_result.end()-str1.begin()) == 6) );

    cv_result=find_token( const_cast<const std::string&>(str1), is_any_of("abc"), token_compress_on );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 3) &&
        ( (cv_result.end()-str1.begin()) == 6) );

    std::string s1("abc def ghi jkl");
    find_iterator<std::string::iterator> fEnd;

    find_iterator<std::string::iterator> fxIt = make_find_iterator(s1,
            token_finder(is_alnum(), token_compress_on));
    HPX_TEST((fxIt != fEnd) && (*fxIt == std::string("abc")));
    ++fxIt;
    HPX_TEST((fxIt != fEnd) && (*fxIt == std::string("def")));
    ++fxIt;
    HPX_TEST((fxIt != fEnd) && (*fxIt == std::string("ghi")));
    ++fxIt;
    HPX_TEST((fxIt != fEnd) && (*fxIt == std::string("jkl")));
    ++fxIt;
    HPX_TEST(fxIt == fEnd);

    nc_result=find_token( str1, is_any_of("abc"), token_compress_off );
    HPX_TEST(
        ( (nc_result.begin()-str1.begin()) == 3) &&
        ( (nc_result.end()-str1.begin()) == 4) );

    cv_result=find_token( const_cast<const std::string&>(str1), is_any_of("abc"), token_compress_off );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 3) &&
        ( (cv_result.end()-str1.begin()) == 4) );

    ch_result=find_token( pch1, is_any_of("abc"), token_compress_off );
    HPX_TEST( ( (ch_result.begin() - pch1 ) == 3 ) && ( (ch_result.end() - pch1 ) == 4 ) );

    // generic find
    nc_result=find(str1, first_finder(std::string("abc")));
    HPX_TEST(
        ( (nc_result.begin()-str1.begin()) == 3) &&
        ( (nc_result.end()-str1.begin()) == 6) );

    cv_result=find(const_cast<const std::string&>(str1), first_finder(str2) );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 3) &&
        ( (cv_result.end()-str1.begin()) == 6) );

    // multi-type comparison test
    nc_vresult=find_first( vec1, std::string("abc") );
    HPX_TEST(
        ( (nc_result.begin()-str1.begin()) == 3) &&
        ( (nc_result.end()-str1.begin()) == 6) );

    cv_vresult=find_first( const_cast<const std::vector<int>&>(vec1), str2 );
    HPX_TEST(
        ( (cv_result.begin()-str1.begin()) == 3) &&
        ( (cv_result.end()-str1.begin()) == 6) );

    // overflow test
    nc_result=find_first( str2, std::string("abcd") );
    HPX_TEST( nc_result.begin()==nc_result.end() );
    cv_result=find_first( const_cast<const std::string&>(str2), std::string("abcd") );
    HPX_TEST( cv_result.begin()==cv_result.end() );

    cv_result=find_head( const_cast<const std::string&>(str2), 4 );
    HPX_TEST( std::string( cv_result.begin(), cv_result.end() )== std::string("abc") );
    cv_result=find_tail( const_cast<const std::string&>(str2), 4 );
    HPX_TEST( std::string( cv_result.begin(), cv_result.end() )== std::string("abc") );

    // Empty std::string test
    nc_result=find_first( str3, std::string("abcd") );
    HPX_TEST( nc_result.begin()==nc_result.end() );
    nc_result=find_first( str1, std::string("") );
    HPX_TEST( nc_result.begin()==nc_result.end() );

    cv_result=find_first( const_cast<const std::string&>(str3), std::string("abcd") );
    HPX_TEST( cv_result.begin()==cv_result.end() );
    cv_result=find_first( const_cast<const std::string&>(str1), std::string("") );
    HPX_TEST( cv_result.begin()==cv_result.end() );

    // iterator_range specific tests
    std::ostringstream osstr;
    osstr << find_first( str1, "abc" );
    HPX_TEST( osstr.str()=="abc" );

}
