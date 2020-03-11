//  Boost string_algo library classification.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef HPX_STRING_CLASSIFICATION_HPP
#define HPX_STRING_CLASSIFICATION_HPP

#include <hpx/string/detail/classification.hpp>
#include <hpx/string/predicate_facade.hpp>

#include <boost/range/as_literal.hpp>
#include <boost/range/value_type.hpp>

#include <algorithm>
#include <locale>

/*! \file
    Classification predicates are included in the library to give
    some more convenience when using algorithms like \c trim() and \c all().
    They wrap functionality of STL classification functions ( e.g. \c std::isspace() )
    into generic functors.
*/

namespace hpx { namespace string {

    //  classification functor generator -------------------------------------//

    //! is_classified predicate
    /*!
            Construct the \c is_classified predicate. This predicate holds if the input is
            of specified \c std::ctype category.

            \param Type A \c std::ctype category
            \param Loc A locale used for classification
            \return An instance of the \c is_classified predicate
        */
    inline detail::is_classifiedF is_classified(
        std::ctype_base::mask Type, const std::locale& Loc = std::locale())
    {
        return detail::is_classifiedF(Type, Loc);
    }

    //! is_space predicate
    /*!
            Construct the \c is_classified predicate for the \c ctype_base::space category.

            \param Loc A locale used for classification
            \return An instance of the \c is_classified predicate
        */
    inline detail::is_classifiedF is_space(
        const std::locale& Loc = std::locale())
    {
        return detail::is_classifiedF(std::ctype_base::space, Loc);
    }

    //! is_alnum predicate
    /*!
            Construct the \c is_classified predicate for the \c ctype_base::alnum category.

            \param Loc A locale used for classification
            \return An instance of the \c is_classified predicate
        */
    inline detail::is_classifiedF is_alnum(
        const std::locale& Loc = std::locale())
    {
        return detail::is_classifiedF(std::ctype_base::alnum, Loc);
    }

    //! is_alpha predicate
    /*!
            Construct the \c is_classified predicate for the \c ctype_base::alpha category.

            \param Loc A locale used for classification
            \return An instance of the \c is_classified predicate
        */
    inline detail::is_classifiedF is_alpha(
        const std::locale& Loc = std::locale())
    {
        return detail::is_classifiedF(std::ctype_base::alpha, Loc);
    }

    //! is_cntrl predicate
    /*!
            Construct the \c is_classified predicate for the \c ctype_base::cntrl category.

            \param Loc A locale used for classification
            \return An instance of the \c is_classified predicate
        */
    inline detail::is_classifiedF is_cntrl(
        const std::locale& Loc = std::locale())
    {
        return detail::is_classifiedF(std::ctype_base::cntrl, Loc);
    }

    //! is_digit predicate
    /*!
            Construct the \c is_classified predicate for the \c ctype_base::digit category.

            \param Loc A locale used for classification
            \return An instance of the \c is_classified predicate
        */
    inline detail::is_classifiedF is_digit(
        const std::locale& Loc = std::locale())
    {
        return detail::is_classifiedF(std::ctype_base::digit, Loc);
    }

    //! is_graph predicate
    /*!
            Construct the \c is_classified predicate for the \c ctype_base::graph category.

            \param Loc A locale used for classification
            \return An instance of the \c is_classified predicate
        */
    inline detail::is_classifiedF is_graph(
        const std::locale& Loc = std::locale())
    {
        return detail::is_classifiedF(std::ctype_base::graph, Loc);
    }

    //! is_lower predicate
    /*!
            Construct the \c is_classified predicate for the \c ctype_base::lower category.

            \param Loc A locale used for classification
            \return An instance of \c is_classified predicate
        */
    inline detail::is_classifiedF is_lower(
        const std::locale& Loc = std::locale())
    {
        return detail::is_classifiedF(std::ctype_base::lower, Loc);
    }

    //! is_print predicate
    /*!
            Construct the \c is_classified predicate for the \c ctype_base::print category.

            \param Loc A locale used for classification
            \return An instance of the \c is_classified predicate
        */
    inline detail::is_classifiedF is_print(
        const std::locale& Loc = std::locale())
    {
        return detail::is_classifiedF(std::ctype_base::print, Loc);
    }

    //! is_punct predicate
    /*!
            Construct the \c is_classified predicate for the \c ctype_base::punct category.

            \param Loc A locale used for classification
            \return An instance of the \c is_classified predicate
        */
    inline detail::is_classifiedF is_punct(
        const std::locale& Loc = std::locale())
    {
        return detail::is_classifiedF(std::ctype_base::punct, Loc);
    }

    //! is_upper predicate
    /*!
            Construct the \c is_classified predicate for the \c ctype_base::upper category.

            \param Loc A locale used for classification
            \return An instance of the \c is_classified predicate
        */
    inline detail::is_classifiedF is_upper(
        const std::locale& Loc = std::locale())
    {
        return detail::is_classifiedF(std::ctype_base::upper, Loc);
    }

    //! is_xdigit predicate
    /*!
            Construct the \c is_classified predicate for the \c ctype_base::xdigit category.

            \param Loc A locale used for classification
            \return An instance of the \c is_classified predicate
        */
    inline detail::is_classifiedF is_xdigit(
        const std::locale& Loc = std::locale())
    {
        return detail::is_classifiedF(std::ctype_base::xdigit, Loc);
    }

    //! is_any_of predicate
    /*!
            Construct the \c is_any_of predicate. The predicate holds if the input
            is included in the specified set of characters.

            \param Set A set of characters to be recognized
            \return An instance of the \c is_any_of predicate
        */
    template <typename RangeT>
    inline detail::is_any_ofF<typename boost::range_value<RangeT>::type>
    is_any_of(const RangeT& Set)
    {
        boost::iterator_range<
            typename boost::range_const_iterator<RangeT>::type>
            lit_set(boost::as_literal(Set));
        return detail::is_any_ofF<typename boost::range_value<RangeT>::type>(
            lit_set);
    }

    //! is_from_range predicate
    /*!
            Construct the \c is_from_range predicate. The predicate holds if the input
            is included in the specified range. (i.e. From <= Ch <= To )

            \param From The start of the range
            \param To The end of the range
            \return An instance of the \c is_from_range predicate
        */
    template <typename CharT>
    inline detail::is_from_rangeF<CharT> is_from_range(CharT From, CharT To)
    {
        return detail::is_from_rangeF<CharT>(From, To);
    }

    // predicate combinators ---------------------------------------------------//

    //! predicate 'and' composition predicate
    /*!
            Construct the \c class_and predicate. This predicate can be used
            to logically combine two classification predicates. \c class_and holds,
            if both predicates return true.

            \param Pred1 The first predicate
            \param Pred2 The second predicate
            \return An instance of the \c class_and predicate
        */
    template <typename Pred1T, typename Pred2T>
    inline detail::pred_andF<Pred1T, Pred2T> operator&&(
        const predicate_facade<Pred1T>& Pred1,
        const predicate_facade<Pred2T>& Pred2)
    {
        // Doing the static_cast with the pointer instead of the reference
        // is a workaround for some compilers which have problems with
        // static_cast's of template references, i.e. CW8. /grafik/
        return detail::pred_andF<Pred1T, Pred2T>(
            *static_cast<const Pred1T*>(&Pred1),
            *static_cast<const Pred2T*>(&Pred2));
    }

    //! predicate 'or' composition predicate
    /*!
            Construct the \c class_or predicate. This predicate can be used
            to logically combine two classification predicates. \c class_or holds,
            if one of the predicates return true.

            \param Pred1 The first predicate
            \param Pred2 The second predicate
            \return An instance of the \c class_or predicate
        */
    template <typename Pred1T, typename Pred2T>
    inline detail::pred_orF<Pred1T, Pred2T> operator||(
        const predicate_facade<Pred1T>& Pred1,
        const predicate_facade<Pred2T>& Pred2)
    {
        // Doing the static_cast with the pointer instead of the reference
        // is a workaround for some compilers which have problems with
        // static_cast's of template references, i.e. CW8. /grafik/
        return detail::pred_orF<Pred1T, Pred2T>(
            *static_cast<const Pred1T*>(&Pred1),
            *static_cast<const Pred2T*>(&Pred2));
    }

    //! predicate negation operator
    /*!
            Construct the \c class_not predicate. This predicate represents a negation.
            \c class_or holds if of the predicates return false.

            \param Pred The predicate to be negated
            \return An instance of the \c class_not predicate
        */
    template <typename PredT>
    inline detail::pred_notF<PredT> operator!(
        const predicate_facade<PredT>& Pred)
    {
        // Doing the static_cast with the pointer instead of the reference
        // is a workaround for some compilers which have problems with
        // static_cast's of template references, i.e. CW8. /grafik/
        return detail::pred_notF<PredT>(*static_cast<const PredT*>(&Pred));
    }

}}    // namespace hpx::string

#endif    // HPX_STRING_PREDICATE_HPP
