//  Boost string_algo library string_traits.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef HPX_STRING_STD_STRING_TRAITS_HPP
#define HPX_STRING_STD_STRING_TRAITS_HPP

#include <hpx/string/yes_no_type.hpp>
#include <hpx/string/sequence_traits.hpp>

#include <string>

namespace hpx {
    namespace string {

//  std::basic_string<> traits  -----------------------------------------------//


    // native replace trait
        template<typename T, typename TraitsT, typename AllocT>
        class has_native_replace< std::basic_string<T, TraitsT, AllocT> >
        {
        public:
            static const bool value = true;
            typedef boost::mpl::bool_<has_native_replace<T>::value> type;
        };



    } // namespace string
} // namespace hpx


#endif  // HPX_STRING_LIST_TRAITS_HPP
