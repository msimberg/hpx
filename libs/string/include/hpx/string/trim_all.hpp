//  Boost string_algo library trim.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
//  SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef HPX_STRING_TRIM_ALL_HPP
#define HPX_STRING_TRIM_ALL_HPP

#include <hpx/config.hpp>
#include <hpx/string/classification.hpp>
#include <hpx/string/find_format.hpp>
#include <hpx/string/finder.hpp>
#include <hpx/string/formatter.hpp>
#include <hpx/string/trim.hpp>

#include <locale>

/*! \file
    Defines trim_all algorithms.

    Just like \c trim, \c trim_all removes all trailing and leading spaces from
    a sequence (string). In addition, spaces in the middle of the sequence are
    truncated to just one character. Space is recognized using given locales.

    \c trim_fill acts as trim_all, but the spaces in the middle are replaces
    with a user-define sequence of character.

    Parametric (\c _if) variants use a predicate (functor) to select which
    characters are to be trimmed.. Functions take a selection predicate as a
    parameter, which is used to determine whether a character is a space. Common
    predicates are provided in classification.hpp header.

*/

namespace hpx { namespace string {

    // multi line trim  ----------------------------------------------- //

    //! Trim All - parametric
    /*!
            Remove all leading and trailing spaces from the input and
            compress all other spaces to a single character.
            The result is a trimmed copy of the input

            \param Input An input sequence
             \param IsSpace A unary predicate identifying spaces
            \return A trimmed copy of the input
        */
    template <typename SequenceT, typename PredicateT>
    inline SequenceT trim_all_copy_if(
        const SequenceT& Input, PredicateT IsSpace)
    {
        return ::hpx::string::find_format_all_copy(
            ::hpx::string::trim_copy_if(Input, IsSpace),
            ::hpx::string::token_finder(
                IsSpace, ::hpx::string::token_compress_on),
            ::hpx::string::dissect_formatter(::hpx::string::head_finder(1)));
    }

    //! Trim All
    /*!
            Remove all leading and trailing spaces from the input and
            compress all other spaces to a single character.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param IsSpace A unary predicate identifying spaces
        */
    template <typename SequenceT, typename PredicateT>
    inline void trim_all_if(SequenceT& Input, PredicateT IsSpace)
    {
        ::hpx::string::trim_if(Input, IsSpace);
        ::hpx::string::find_format_all(Input,
            ::hpx::string::token_finder(
                IsSpace, ::hpx::string::token_compress_on),
            ::hpx::string::dissect_formatter(::hpx::string::head_finder(1)));
    }

    //! Trim All
    /*!
            Remove all leading and trailing spaces from the input and
            compress all other spaces to a single character.
            The result is a trimmed copy of the input

            \param Input An input sequence
            \param Loc A locale used for 'space' classification
            \return A trimmed copy of the input
        */
    template <typename SequenceT>
    inline SequenceT trim_all_copy(
        const SequenceT& Input, const std::locale& Loc = std::locale())
    {
        return trim_all_copy_if(Input, ::hpx::string::is_space(Loc));
    }

    //! Trim All
    /*!
            Remove all leading and trailing spaces from the input and
            compress all other spaces to a single character.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param Loc A locale used for 'space' classification
            \return A trimmed copy of the input
        */
    template <typename SequenceT>
    inline void trim_all(
        SequenceT& Input, const std::locale& Loc = std::locale())
    {
        trim_all_if(Input, ::hpx::string::is_space(Loc));
    }

    //! Trim Fill - parametric
    /*!
            Remove all leading and trailing spaces from the input and
            replace all every block of consecutive spaces with a fill string
            defined by user.
            The result is a trimmed copy of the input

            \param Input An input sequence
            \param Fill A string used to fill the inner spaces
            \param IsSpace A unary predicate identifying spaces
            \return A trimmed copy of the input
        */
    template <typename SequenceT, typename RangeT, typename PredicateT>
    inline SequenceT trim_fill_copy_if(
        const SequenceT& Input, const RangeT& Fill, PredicateT IsSpace)
    {
        return ::hpx::string::find_format_all_copy(
            ::hpx::string::trim_copy_if(Input, IsSpace),
            ::hpx::string::token_finder(
                IsSpace, ::hpx::string::token_compress_on),
            ::hpx::string::const_formatter(::boost::as_literal(Fill)));
    }

    //! Trim Fill
    /*!
            Remove all leading and trailing spaces from the input and
            replace all every block of consecutive spaces with a fill string
            defined by user.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param Fill A string used to fill the inner spaces
            \param IsSpace A unary predicate identifying spaces
        */
    template <typename SequenceT, typename RangeT, typename PredicateT>
    inline void trim_fill_if(
        SequenceT& Input, const RangeT& Fill, PredicateT IsSpace)
    {
        ::hpx::string::trim_if(Input, IsSpace);
        ::hpx::string::find_format_all(Input,
            ::hpx::string::token_finder(
                IsSpace, ::hpx::string::token_compress_on),
            ::hpx::string::const_formatter(::boost::as_literal(Fill)));
    }

    //! Trim Fill
    /*!
            Remove all leading and trailing spaces from the input and
            replace all every block of consecutive spaces with a fill string
            defined by user.
            The result is a trimmed copy of the input

            \param Input An input sequence
            \param Fill A string used to fill the inner spaces
            \param Loc A locale used for 'space' classification
            \return A trimmed copy of the input
        */
    template <typename SequenceT, typename RangeT>
    inline SequenceT trim_fill_copy(const SequenceT& Input, const RangeT& Fill,
        const std::locale& Loc = std::locale())
    {
        return trim_fill_copy_if(Input, Fill, ::hpx::string::is_space(Loc));
    }

    //! Trim Fill
    /*!
            Remove all leading and trailing spaces from the input and
            replace all every block of consecutive spaces with a fill string
            defined by user.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param Fill A string used to fill the inner spaces
            \param Loc A locale used for 'space' classification
            \return A trimmed copy of the input
        */
    template <typename SequenceT, typename RangeT>
    inline void trim_fill(SequenceT& Input, const RangeT& Fill,
        const std::locale& Loc = std::locale())
    {
        trim_fill_if(Input, Fill, ::hpx::string::is_space(Loc));
    }

}}    // namespace hpx::string

#endif    // HPX_STRING_TRIM_ALL_HPP
