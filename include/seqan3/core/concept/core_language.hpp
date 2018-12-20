// ============================================================================
//                 SeqAn - The Library for Sequence Analysis
// ============================================================================
//
// Copyright (c) 2006-2018, Knut Reinert & Freie Universitaet Berlin
// Copyright (c) 2016-2018, Knut Reinert & MPI Molekulare Genetik
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Knut Reinert or the FU Berlin nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL KNUT REINERT OR THE FU BERLIN BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ============================================================================

/*!\file
 * \brief Provides concepts for core language types and relations that don't have concepts in C++20 (yet).
 * \author Hannes Hauswedell <hannes.hauswedell AT fu-berlin.de>
 */

#pragma once

#include <type_traits>

#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>

#include <seqan3/core/platform.hpp>
#include <seqan3/std/concepts>
#include <seqan3/std/ranges>

namespace seqan3::detail
{

/*!\addtogroup concept
 * \{
 */

/*!\interface   seqan3::detail::weakly_equality_comparable_by_members_with_concept <>
 * \brief       Like std::detail::WeaklyEqualityComparableWith, but considers only member operators of the LHS.
 */
//!\cond
template <typename lhs_t, typename rhs_t>
concept weakly_equality_comparable_by_members_with_concept = requires (lhs_t const & lhs, rhs_t const & rhs)
{
    lhs.operator==(rhs); std::Boolean<decltype(lhs.operator==(rhs))>;
    lhs.operator!=(rhs); std::Boolean<decltype(lhs.operator!=(rhs))>;
};
//!\endcond
/*!\interface   seqan3::detail::weakly_ordered_by_members_with_concept <>
 * \brief       Like seqan3::weakly_ordered_with_concept, but considers only member operators of the LHS.
 */
//!\cond
template <typename lhs_t, typename rhs_t>
concept weakly_ordered_by_members_with_concept = requires (lhs_t const & lhs, rhs_t const & rhs)
{
    lhs.operator< (rhs); std::Boolean<decltype(lhs.operator< (rhs))>;
    lhs.operator> (rhs); std::Boolean<decltype(lhs.operator> (rhs))>;
    lhs.operator<=(rhs); std::Boolean<decltype(lhs.operator<=(rhs))>;
    lhs.operator>=(rhs); std::Boolean<decltype(lhs.operator>=(rhs))>;
};
//!\endcond

/*!\interface   seqan3::detail::convertible_to_by_member_concept <>
 * \brief       Like seqan3::implicitly_convertible_to_concept, but only considers member operators of the source type.
 */
//!\cond
template <typename source_t, typename target_t>
concept convertible_to_by_member_concept = requires (source_t s)
{
    { s.operator target_t() } -> target_t;
};
//!\endcond
//!\}

} // seqan3::detail

namespace seqan3
{

/*!\addtogroup concept
 * \{
 */

/*!\interface   seqan3::weakly_ordered_with_concept <>
 * \tparam t1   The first type to compare.
 * \tparam t2   The second type to compare.
 * \brief       Requires the two operands to be comparable with `==` and `!=` in both directions.
 * \sa          http://en.cppreference.com/w/cpp/experimental/ranges/concepts/WeaklyEqualityComparableWith
 */
//!\cond
template <typename t1, typename t2>
concept weakly_ordered_with_concept = requires (std::remove_reference_t<t1> const & v1,
                                                     std::remove_reference_t<t2> const & v2)
{
    { v1 <  v2 } -> bool &&;
    { v1 <= v2 } -> bool &&;
    { v2 >  v1 } -> bool &&;
    { v2 >= v1 } -> bool &&;
};
//!\endcond

/*!\interface   seqan3::implicitly_convertible_to_concept <>
 * \brief       Resolves to `std::ranges::ImplicitlyConvertibleTo<type1, type2>()`.
 */
//!\cond
template <typename t, typename u>
concept implicitly_convertible_to_concept = std::is_convertible_v<t, u>;
//!\endcond

/*!\interface   seqan3::explicitly_convertible_to_concept <>
 * \brief       Resolves to `std::ranges::ExplicitlyConvertibleTo<type1, type2>()`.
 */
//!\cond
template <typename t, typename u>
concept explicitly_convertible_to_concept = requires (t vt) { { static_cast<u>(vt)}; };
//!\endcond

/*!\interface   seqan3::arithmetic_concept <>
 * \brief       A type that satisfies std::is_arithmetic_v<t>.
 * \sa          http://en.cppreference.com/w/cpp/types/is_arithmetic
 */
//!\cond
template <typename t>
concept arithmetic_concept = std::is_arithmetic_v<t>;
//!\endcond

/*!\interface   seqan3::floating_point_concept <>
 * \extends     seqan3::arithmetic_concept
 * \brief       An arithmetic type that also satisfies std::is_floating_point_v<t>.
 * \sa          http://en.cppreference.com/w/cpp/types/is_floating_point
 */
//!\cond
template <typename t>
concept floating_point_concept = arithmetic_concept<t> && std::is_floating_point_v<t>;
//!\endcond

/*!\interface   seqan3::char_concept <>
 * \extends     std::Integral
 * \brief       This concept encompasses exactly the types `char`, `signed char`, `unsigned char`, `wchar_t`,
 *              `char16_t` and `char32_t`.
 */
//!\cond

template <typename t>
concept char_concept = std::Integral<t> &&
                       (std::Same<t, char> || std::Same<t, unsigned char> || std::Same<t, signed char> ||
#ifdef __cpp_char8_t
                        std::Same<t, char8_t> ||
#endif
                        std::Same<t, char16_t> || std::Same<t, char32_t> || std::Same<t, wchar_t>);
//!\endcond

/*!\interface   seqan3::trivially_destructible_concept <>
 * \extends     std::Destructible
 * \brief       A type that satisfies std::is_trivially_destructible_v<t>.
 * \sa          http://en.cppreference.com/w/cpp/types/is_destructible
 */
//!\cond
template <typename t>
concept trivially_destructible_concept = std::Destructible<t> && std::is_trivially_destructible_v<t>;
//!\endcond

/*!\interface   seqan3::trivially_copyable_concept
 * \brief       A type that satisfies std::is_trivially_copyable_v<t>.
 * \extends     std::Copyable
 * \sa          http://en.cppreference.com/w/cpp/types/is_trivially_copyable
 */
//!\cond
template <typename t>
concept trivially_copyable_concept = std::Copyable<t> && std::is_trivially_copyable_v<t>;
//!\endcond

/*!\interface   seqan3::trivial_concept
 * \brief       A type that satisfies seqan3::trivially_copyable_concept and seqan3::trivially_destructible_concept.
 * \extends     seqan3::trivially_copyable_concept
 * \extends     seqan3::trivially_destructible_concept
 * \sa          http://en.cppreference.com/w/cpp/experimental/ranges/concepts/Copyable
 */
//!\cond
template <typename t>
concept trivial_concept = trivially_copyable_concept<t> && trivially_destructible_concept<t> && std::is_trivial_v<t>;
//!\endcond

/*!\interface   seqan3::standard_layout_concept
 * \brief       Resolves to std::is_standard_layout_v<t>.
 * \sa          http://en.cppreference.com/w/cpp/concept/StandardLayoutType
 */
//!\cond
template <typename t>
concept standard_layout_concept = std::is_standard_layout_v<t>;
//!\endcond

/*!\interface   seqan3::weakly_assignable_concept
 * \brief       Resolves to std::is_assignable_v<t>.
 * \sa          https://en.cppreference.com/w/cpp/types/is_assignable
 *
 * \details
 *
 * Note that this requires less than std::Assignable, it simply tests if the expression
 * `std::declval<T>() = std::declval<U>()` is well-formed.
 */
//!\cond
template <typename t, typename u>
concept weakly_assignable_concept = std::is_assignable_v<t, u>;
//!\endcond

}  // namespace seqan3
