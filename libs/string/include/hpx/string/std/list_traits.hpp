//  Boost string_algo library list_traits.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
//  SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

//  hpxinspect:nodeprecatedname:boost::mpl

#ifndef HPX_STRING_STD_LIST_TRAITS_HPP
#define HPX_STRING_STD_LIST_TRAITS_HPP

#include <hpx/string/sequence_traits.hpp>
#include <hpx/string/yes_no_type.hpp>

#include <list>

namespace hpx { namespace string {

    //  std::list<> traits  -----------------------------------------------//

    // stable iterators trait
    template <typename T, typename AllocT>
    class has_stable_iterators<::std::list<T, AllocT>>
    {
    public:
        static const bool value = true;
        typedef boost::mpl::bool_<has_stable_iterators<T>::value> type;
    };

    // const time insert trait
    template <typename T, typename AllocT>
    class has_const_time_insert<::std::list<T, AllocT>>
    {
    public:
        static const bool value = true;
        typedef boost::mpl::bool_<has_const_time_insert<T>::value> type;
    };

    // const time erase trait
    template <typename T, typename AllocT>
    class has_const_time_erase<::std::list<T, AllocT>>
    {
    public:
        static const bool value = true;
        typedef boost::mpl::bool_<has_const_time_erase<T>::value> type;
    };

}}    // namespace hpx::string

#endif    // HPX_STRING_STD_LIST_TRAITS_HPP
