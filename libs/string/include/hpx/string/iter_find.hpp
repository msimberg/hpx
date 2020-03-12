//  Boost string_algo library iter_find.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
//  SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

//  hpxinspect:nodeprecatedname:boost::iterator_range

#ifndef HPX_STRING_ITER_FIND_HPP
#define HPX_STRING_ITER_FIND_HPP

#include <hpx/config.hpp>
#include <hpx/string/concept.hpp>
#include <hpx/string/detail/util.hpp>
#include <hpx/string/find_iterator.hpp>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/as_literal.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/iterator.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/range/value_type.hpp>

#include <algorithm>
#include <iterator>

/*! \file
    Defines generic split algorithms. Split algorithms can be
    used to divide a sequence into several part according
    to a given criteria. Result is given as a 'container
    of containers' where elements are copies or references
    to extracted parts.

    There are two algorithms provided. One iterates over matching
    substrings, the other one over the gaps between these matches.
*/

namespace hpx { namespace string {

    //  iterate find ---------------------------------------------------//

    //! Iter find algorithm
    /*!
            This algorithm executes a given finder in iteration on the input,
            until the end of input is reached, or no match is found.
            Iteration is done using built-in find_iterator, so the real
            searching is performed only when needed.
            In each iteration new match is found and added to the result.

            \param Result A 'container container' to contain the result of search.
                Both outer and inner container must have constructor taking a pair
                of iterators as an argument.
                Typical type of the result is
                    \c std::vector<boost::iterator_range<iterator>>
                (each element of such a vector will container a range delimiting
                a match).
            \param Input A container which will be searched.
            \param Finder A Finder object used for searching
            \return A reference to the result

            \note Prior content of the result will be overwritten.
        */
    template <typename SequenceSequenceT, typename RangeT, typename FinderT>
    inline SequenceSequenceT& iter_find(SequenceSequenceT& Result,
#if !defined(HPX_NO_CXX11_RVALUE_REFERENCES)
        RangeT&& Input,
#else
        RangeT& Input,
#endif
        FinderT Finder)
    {
        BOOST_CONCEPT_ASSERT((FinderConcept<FinderT,
            typename boost::range_iterator<RangeT>::type>) );

        boost::iterator_range<typename boost::range_iterator<RangeT>::type>
            lit_input(::boost::as_literal(Input));

        typedef
            typename boost::range_iterator<RangeT>::type input_iterator_type;
        typedef find_iterator<input_iterator_type> find_iterator_type;
        typedef detail::copy_iterator_rangeF<
            typename boost::range_value<SequenceSequenceT>::type,
            input_iterator_type>
            copy_range_type;

        input_iterator_type InputEnd = ::boost::end(lit_input);

        typedef boost::transform_iterator<copy_range_type, find_iterator_type>
            transform_iter_type;

        transform_iter_type itBegin = ::boost::make_transform_iterator(
            find_iterator_type(::boost::begin(lit_input), InputEnd, Finder),
            copy_range_type());

        transform_iter_type itEnd = ::boost::make_transform_iterator(
            find_iterator_type(), copy_range_type());

        SequenceSequenceT Tmp(itBegin, itEnd);

        Result.swap(Tmp);
        return Result;
    }

    //  iterate split ---------------------------------------------------//

    //! Split find algorithm
    /*!
            This algorithm executes a given finder in iteration on the input,
            until the end of input is reached, or no match is found.
            Iteration is done using built-in find_iterator, so the real
            searching is performed only when needed.
            Each match is used as a separator of segments. These segments are then
            returned in the result.

            \param Result A 'container container' to contain the result of search.
                Both outer and inner container must have constructor taking a pair
                of iterators as an argument.
                Typical type of the result is
                    \c std::vector<boost::iterator_range<iterator>>
                (each element of such a vector will container a range delimiting
                a match).
            \param Input A container which will be searched.
            \param Finder A finder object used for searching
            \return A reference to the result

            \note Prior content of the result will be overwritten.
        */
    template <typename SequenceSequenceT, typename RangeT, typename FinderT>
    inline SequenceSequenceT& iter_split(SequenceSequenceT& Result,
#if !defined(HPX_NO_CXX11_RVALUE_REFERENCES)
        RangeT&& Input,
#else
        RangeT& Input,
#endif
        FinderT Finder)
    {
        BOOST_CONCEPT_ASSERT((FinderConcept<FinderT,
            typename boost::range_iterator<RangeT>::type>) );

        boost::iterator_range<typename boost::range_iterator<RangeT>::type>
            lit_input(::boost::as_literal(Input));

        typedef
            typename boost::range_iterator<RangeT>::type input_iterator_type;
        typedef split_iterator<input_iterator_type> find_iterator_type;
        typedef detail::copy_iterator_rangeF<
            typename boost::range_value<SequenceSequenceT>::type,
            input_iterator_type>
            copy_range_type;

        input_iterator_type InputEnd = ::boost::end(lit_input);

        typedef boost::transform_iterator<copy_range_type, find_iterator_type>
            transform_iter_type;

        transform_iter_type itBegin = ::boost::make_transform_iterator(
            find_iterator_type(::boost::begin(lit_input), InputEnd, Finder),
            copy_range_type());

        transform_iter_type itEnd = ::boost::make_transform_iterator(
            find_iterator_type(), copy_range_type());

        SequenceSequenceT Tmp(itBegin, itEnd);

        Result.swap(Tmp);
        return Result;
    }

}}    // namespace hpx::string

#endif    // HPX_STRING_ITER_FIND_HPP
