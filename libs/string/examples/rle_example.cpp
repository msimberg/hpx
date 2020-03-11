//  Boost string_algo library example file  ---------------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

/*
    RLE compression using replace framework. Goal is to compress a sequence of
    repeating characters into 3 bytes ( repeat mark, character and repetition count ).
    For simplification, it works only on numeric-value sequences.
*/

#include <hpx/string/find_format.hpp>
#include <hpx/string/finder.hpp>

#include <boost/detail/iterator.hpp>

#include <iostream>
#include <limits>
#include <string>

// replace mark specification, specialize for a specific element type
template <typename T>
T repeat_mark()
{
    return (std::numeric_limits<T>::max)();
};

// Compression  -----------------------------------------------------------------------

// compress finder -rle
/*
    Find a sequence which can be compressed. It has to be at least 3-character long
    sequence of repetitive characters
*/
struct find_compressF
{
    // Construction
    find_compressF() {}

    // Operation
    template <typename ForwardIteratorT>
    boost::iterator_range<ForwardIteratorT> operator()(
        ForwardIteratorT Begin, ForwardIteratorT End) const
    {
        typedef ForwardIteratorT input_iterator_type;
        typedef typename boost::detail::iterator_traits<
            input_iterator_type>::value_type value_type;
        typedef boost::iterator_range<input_iterator_type> result_type;

        // begin of the matching segment
        input_iterator_type MStart = End;
        // Repetition counter
        value_type Cnt = 0;

        // Search for a sequence of repetitive characters
        for (input_iterator_type It = Begin; It != End;)
        {
            input_iterator_type It2 = It++;

            if (It == End || Cnt >= (std::numeric_limits<value_type>::max)())
            {
                return result_type(MStart, It);
            }

            if (*It == *It2)
            {
                if (MStart == End)
                {
                    // Mark the start
                    MStart = It2;
                }

                // Increate repetition counter
                Cnt++;
            }
            else
            {
                if (MStart != End)
                {
                    if (Cnt > 2)
                        return result_type(MStart, It);
                    else
                    {
                        MStart = End;
                        Cnt = 0;
                    }
                }
            }
        }

        return result_type(End, End);
    }
};

// rle compress format
/*
    Transform a sequence into repeat mark, character and count
*/
template <typename SeqT>
struct format_compressF
{
private:
    typedef SeqT result_type;
    typedef typename SeqT::value_type value_type;

public:
    // Construction
    format_compressF(){};

    // Operation
    template <typename ReplaceT>
    result_type operator()(const ReplaceT& Replace) const
    {
        SeqT r;
        if (!Replace.empty())
        {
            r.push_back(repeat_mark<value_type>());
            r.push_back(*(Replace.begin()));
            r.push_back(value_type(Replace.size()));
        }

        return r;
    }
};

// Decompression  -----------------------------------------------------------------------

// find decompress-rle functor
/*
    find a repetition block
*/
struct find_decompressF
{
    // Construction
    find_decompressF() {}

    // Operation
    template <typename ForwardIteratorT>
    boost::iterator_range<ForwardIteratorT> operator()(
        ForwardIteratorT Begin, ForwardIteratorT End) const
    {
        typedef ForwardIteratorT input_iterator_type;
        typedef typename boost::detail::iterator_traits<
            input_iterator_type>::value_type value_type;
        typedef boost::iterator_range<input_iterator_type> result_type;

        for (input_iterator_type It = Begin; It != End; It++)
        {
            if (*It == repeat_mark<value_type>())
            {
                // Repeat mark found, extract body
                input_iterator_type It2 = It++;

                if (It == End)
                    break;
                It++;
                if (It == End)
                    break;
                It++;

                return result_type(It2, It);
            }
        }

        return result_type(End, End);
    }
};

// rle decompress format
/*
    transform a repetition block into a sequence of characters
*/
template <typename SeqT>
struct format_decompressF
{
private:
    typedef SeqT result_type;
    typedef typename SeqT::value_type value_type;

public:
    // Construction
    format_decompressF(){};

    // Operation
    template <typename ReplaceT>
    result_type operator()(const ReplaceT& Replace) const
    {
        SeqT r;

        if (!Replace.empty())
        {
            // extract info
            typename ReplaceT::const_iterator It = Replace.begin();

            value_type Value = *(++It);
            value_type Repeat = *(++It);

            for (value_type Index = 0; Index < Repeat; Index++)
                r.push_back(Value);
        }

        return r;
    }
};

int main()
{
    std::cout << "* RLE Compression Example *" << std::endl << std::endl;

    std::string original("123_AA_*ZZZZZZZZZZZZZZ*34");

    // copy compression
    std::string compress = hpx::string::find_format_all_copy(
        original, find_compressF(), format_compressF<std::string>());

    std::cout << "Compressed string: " << compress << std::endl;

    // Copy decompression
    std::string decompress = hpx::string::find_format_all_copy(
        compress, find_decompressF(), format_decompressF<std::string>());

    std::cout << "Decompressed string: " << decompress << std::endl;

    // in-place compression
    hpx::string::find_format_all(
        original, find_compressF(), format_compressF<std::string>());

    std::cout << "Compressed string: " << original << std::endl;

    // in-place decompression
    hpx::string::find_format_all(
        original, find_decompressF(), format_decompressF<std::string>());

    std::cout << "Decompressed string: " << original << std::endl;

    std::cout << std::endl;

    return 0;
}
