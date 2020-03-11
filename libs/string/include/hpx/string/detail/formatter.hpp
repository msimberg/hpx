//  Boost string_algo library formatter.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#ifndef HPX_STRING_FORMATTER_DETAIL_HPP
#define HPX_STRING_FORMATTER_DETAIL_HPP

#include <hpx/string/detail/util.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/const_iterator.hpp>
#include <boost/range/end.hpp>
#include <boost/range/iterator_range_core.hpp>

//  generic replace functors -----------------------------------------------//

namespace hpx { namespace string { namespace detail {

    //  const format functor ----------------------------------------------------//

    // constant format functor
    template <typename RangeT>
    struct const_formatF
    {
    private:
        typedef
            typename boost::range_const_iterator<RangeT>::type format_iterator;
        typedef boost::iterator_range<format_iterator> result_type;

    public:
        // Construction
        const_formatF(const RangeT& Format)
          : m_Format(::boost::begin(Format), ::boost::end(Format))
        {
        }

        // Operation
        template <typename Range2T>
        result_type& operator()(const Range2T&)
        {
            return m_Format;
        }

        template <typename Range2T>
        const result_type& operator()(const Range2T&) const
        {
            return m_Format;
        }

    private:
        result_type m_Format;
    };

    //  identity format functor ----------------------------------------------------//

    // identity format functor
    template <typename RangeT>
    struct identity_formatF
    {
        // Operation
        template <typename Range2T>
        const RangeT& operator()(const Range2T& Replace) const
        {
            return RangeT(::boost::begin(Replace), ::boost::end(Replace));
        }
    };

    //  empty format functor ( used by erase ) ------------------------------------//

    // empty format functor
    template <typename CharT>
    struct empty_formatF
    {
        template <typename ReplaceT>
        empty_container<CharT> operator()(const ReplaceT&) const
        {
            return empty_container<CharT>();
        }
    };

    //  dissect format functor ----------------------------------------------------//

    // dissect format functor
    template <typename FinderT>
    struct dissect_formatF
    {
    public:
        // Construction
        dissect_formatF(FinderT Finder)
          : m_Finder(Finder)
        {
        }

        // Operation
        template <typename RangeT>
        inline boost::iterator_range<
            typename boost::range_const_iterator<RangeT>::type>
        operator()(const RangeT& Replace) const
        {
            return m_Finder(::boost::begin(Replace), ::boost::end(Replace));
        }

    private:
        FinderT m_Finder;
    };

}}}    // namespace hpx::string::detail

#endif    // HPX_STRING_FORMATTER_DETAIL_HPP
