//  Boost string_algo library case_conv.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
//  SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef HPX_STRING_CASE_CONV_HPP
#define HPX_STRING_CASE_CONV_HPP

#include <hpx/config.hpp>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/as_literal.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/value_type.hpp>

#include <algorithm>
#include <locale>

#include <hpx/string/detail/case_conv.hpp>

/*! \file
    Defines sequence case-conversion algorithms.
    Algorithms convert each element in the input sequence to the
    desired case using provided locales.
*/

namespace hpx { namespace string {

    //  to_lower  -----------------------------------------------//

    //! Convert to lower case
    /*!
            Each element of the input sequence is converted to lower case. The
            result is a copy of the input converted to lower case. It is
            returned as a sequence or copied to the output iterator.

            \param Output An output iterator to which the result will be copied
            \param Input An input range
            \param Loc A locale used for conversion
            \return An output iterator pointing just after the last inserted
                character or a copy of the input

            \note The second variant of this function provides the strong
                exception-safety guarantee

        */
    template <typename OutputIteratorT, typename RangeT>
    inline OutputIteratorT to_lower_copy(OutputIteratorT Output,
        const RangeT& Input, const std::locale& Loc = std::locale())
    {
        return ::hpx::string::detail::transform_range_copy(Output,
            ::boost::as_literal(Input),
            ::hpx::string::detail::to_lowerF<
                typename boost::range_value<RangeT>::type>(Loc));
    }

    //! Convert to lower case
    /*!
            \overload
        */
    template <typename SequenceT>
    inline SequenceT to_lower_copy(
        const SequenceT& Input, const std::locale& Loc = std::locale())
    {
        return ::hpx::string::detail::transform_range_copy<SequenceT>(Input,
            ::hpx::string::detail::to_lowerF<
                typename boost::range_value<SequenceT>::type>(Loc));
    }

    //! Convert to lower case
    /*!
            Each element of the input sequence is converted to lower
            case. The input sequence is modified in-place.

            \param Input A range
            \param Loc a locale used for conversion
        */
    template <typename WritableRangeT>
    inline void to_lower(
        WritableRangeT& Input, const std::locale& Loc = std::locale())
    {
        ::hpx::string::detail::transform_range(::boost::as_literal(Input),
            ::hpx::string::detail::to_lowerF<
                typename boost::range_value<WritableRangeT>::type>(Loc));
    }

    //  to_upper  -----------------------------------------------//

    //! Convert to upper case
    /*!
            Each element of the input sequence is converted to upper case. The
            result is a copy of the input converted to upper case. It is
            returned as a sequence or copied to the output iterator

            \param Output An output iterator to which the result will be copied
            \param Input An input range
            \param Loc A locale used for conversion
            \return An output iterator pointing just after the last inserted
                character or a copy of the input

            \note The second variant of this function provides the strong
                exception-safety guarantee
        */
    template <typename OutputIteratorT, typename RangeT>
    inline OutputIteratorT to_upper_copy(OutputIteratorT Output,
        const RangeT& Input, const std::locale& Loc = std::locale())
    {
        return ::hpx::string::detail::transform_range_copy(Output,
            ::boost::as_literal(Input),
            ::hpx::string::detail::to_upperF<
                typename boost::range_value<RangeT>::type>(Loc));
    }

    //! Convert to upper case
    /*!
            \overload
        */
    template <typename SequenceT>
    inline SequenceT to_upper_copy(
        const SequenceT& Input, const std::locale& Loc = std::locale())
    {
        return ::hpx::string::detail::transform_range_copy<SequenceT>(Input,
            ::hpx::string::detail::to_upperF<
                typename boost::range_value<SequenceT>::type>(Loc));
    }

    //! Convert to upper case
    /*!
            Each element of the input sequence is converted to upper
            case. The input sequence is modified in-place.

            \param Input An input range
            \param Loc a locale used for conversion
        */
    template <typename WritableRangeT>
    inline void to_upper(
        WritableRangeT& Input, const std::locale& Loc = std::locale())
    {
        ::hpx::string::detail::transform_range(::boost::as_literal(Input),
            ::hpx::string::detail::to_upperF<
                typename boost::range_value<WritableRangeT>::type>(Loc));
    }

}}    // namespace hpx::string

#endif    // HPX_STRING_CASE_CONV_HPP
