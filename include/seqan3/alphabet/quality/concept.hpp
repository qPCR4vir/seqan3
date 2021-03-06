// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \author Marie Hoffmann <marie.hoffmann AT fu-berlin.de>
 * \author Jörg Winkler <j.winkler AT fu-berlin.de>
 * \brief Quality alphabet concept.
 */

#pragma once

#include <seqan3/alphabet/concept.hpp>

// ============================================================================
// to_phred()
// ============================================================================

namespace seqan3::detail::adl_only
{

//!\brief Poison-pill overload to prevent non-ADL forms of unqualified lookup.
template <typename ...args_t>
void to_phred(args_t ...) = delete;

//!\brief seqan3::detail::customisation_point_object (CPO) definition for seqan3::to_phred.
//!\ingroup quality
struct to_phred_cpo : public detail::customisation_point_object<to_phred_cpo, 2>
{
    //!\brief CRTP base class seqan3::detail::customisation_point_object.
    using base_t = detail::customisation_point_object<to_phred_cpo, 2>;
    //!\brief Only this class is allowed to import the constructors from #base_t. (CRTP safety idiom)
    using base_t::base_t;

    /*!\brief CPO overload (1. out of 3 checks): explicit customisation via `seqan3::custom::alphabet`
     * \tparam alphabet_t The type of the alphabet.
     * \param alphabet The alphabet the Phred is returned from.
     */
    template <typename alphabet_t>
    static constexpr auto SEQAN3_CPO_OVERLOAD(priority_tag<2>, alphabet_t && alphabet)
    (
        /*return*/ seqan3::custom::alphabet<alphabet_t>::to_phred(std::forward<alphabet_t>(alphabet)) /*;*/
    );

    /*!\brief CPO overload (2. out of 3 checks): argument dependent lookup (ADL), i.e. `to_phred(alphabet)`
     * \tparam alphabet_t The type of the alphabet.
     * \param alphabet The alphabet the Phred is returned from.
     */
    template <typename alphabet_t>
    static constexpr auto SEQAN3_CPO_OVERLOAD(priority_tag<1>, alphabet_t && alphabet)
    (
        /*return*/ to_phred(std::forward<alphabet_t>(alphabet)) /*;*/
    );

    /*!\brief CPO overload (3. out of 3 checks): member access, i.e. `alphabet.to_phred()`
     * \tparam alphabet_t The type of the alphabet.
     * \param alphabet The alphabet the Phred is returned from.
     */
    template <typename alphabet_t>
    static constexpr auto SEQAN3_CPO_OVERLOAD(priority_tag<0>, alphabet_t && alphabet)
    (
        /*return*/ std::forward<alphabet_t>(alphabet).to_phred() /*;*/
    );
};

} // namespace seqan3::detail::adl_only

namespace seqan3
{

/*!\name Function objects (Quality)
 * \{
 */

/*!\brief The public getter function for the Phred representation of a quality score.
 * \tparam your_type The type of alphabet. Must model the seqan3::quality_alphabet.
 * \param  chr       The quality value to convert into the Phred score.
 * \returns the Phred representation of a quality score.
 * \ingroup quality
 *
 * \details
 *
 * This is a function object. Invoke it with the parameter(s) specified above.
 *
 * It acts as a wrapper and looks for three possible implementations (in this order):
 *
 *   1. A static member function `to_phred(your_type const a)` of the class `seqan3::custom::alphabet<your_type>`.
 *   2. A free function `to_phred(your_type const a)` in the namespace of your type (or as `friend`).
 *   3. A member function called `to_phred()`.
 *
 * Functions are only considered for one of the above cases if they are marked `noexcept` (`constexpr` is not required,
 * but recommended) and if the returned type is convertible to `size_t`.
 *
 * Every quality alphabet type must provide one of the above.
 *
 * ### Customisation point
 *
 * This is a customisation point (see \ref about_customisation). To specify the behaviour for your own alphabet type,
 * simply provide one of the three functions specified above.
 */
inline constexpr auto to_phred = detail::adl_only::to_phred_cpo{};
//!\}

/*!\brief The `phred_type` of the alphabet; defined as the return type of seqan3::to_phred.
 * \ingroup quality
 */
template <typename alphabet_type>
//!\cond
    requires requires { { seqan3::to_phred(std::declval<alphabet_type>()) }; }
//!\endcond
using alphabet_phred_t = decltype(seqan3::to_phred(std::declval<alphabet_type>()));

} // namespace seqan3

// ============================================================================
// assign_phred_to()
// ============================================================================

namespace seqan3::detail::adl_only
{

//!\brief Poison-pill overload to prevent non-ADL forms of unqualified lookup.
template <typename ...args_t>
void assign_phred_to(args_t ...) = delete;

//!\brief Functor definition for seqan3::assign_phred_to.
//!\ingroup quality
struct assign_phred_to_fn
{
public:
    SEQAN3_CPO_IMPL(2, (seqan3::custom::alphabet<decltype(v)>::assign_phred_to(args..., v))) // explicit customisation
    SEQAN3_CPO_IMPL(1, (assign_phred_to(args..., v)                                       )) // ADL
    SEQAN3_CPO_IMPL(0, (v.assign_phred(args...)                                           )) // member

public:
    //!\brief Operator definition for lvalues.
    template <typename alph_t>
    //!\cond
        requires requires (seqan3::alphabet_phred_t<alph_t> const p, alph_t & a)
            { { impl(priority_tag<2>{}, a, p) }; }
    //!\endcond
    constexpr alph_t & operator()(seqan3::alphabet_phred_t<alph_t> const p, alph_t & a) const noexcept
    {
        static_assert(noexcept(impl(priority_tag<2>{}, a, p)),
            "Only overloads that are marked noexcept are picked up by seqan3::assign_phred_to().");
        static_assert(std::same_as<alph_t &, decltype(impl(priority_tag<2>{}, a, p))>,
            "The return type of your assign_phred_to() implementation must be 'alph_t &'.");

        return impl(priority_tag<2>{}, a, p);
    }

    //!\brief Operator definition for rvalues.
    template <typename alph_t>
    //!\cond
        requires requires (seqan3::alphabet_phred_t<alph_t> const p, alph_t & a)
            { { impl(priority_tag<2>{}, a, p) }; } && (!std::is_lvalue_reference_v<alph_t>)
    //!\endcond
    constexpr alph_t operator()(seqan3::alphabet_phred_t<alph_t> const p, alph_t && a) const noexcept
    {
        return (*this)(p, a); // call above function but return by value
    }
};

} // namespace seqan3::detail::adl_only

namespace seqan3
{

/*!\name Function objects (Quality)
 * \{
 */

/*!\brief Assign a Phred score to a quality alphabet object.
 * \tparam your_type The type of the target object. Must model the seqan3::quality_alphabet.
 * \param  chr       The Phred score being assigned; must be of the seqan3::alphabet_phred_t of the target object.
 * \returns Reference to `alph` if `alph` was given as lvalue, otherwise a copy.
 * \ingroup quality
 * \details
 *
 * This is a function object. Invoke it with the parameter(s) specified above.
 *
 * It acts as a wrapper and looks for three possible implementations (in this order):
 *
 *   1. A static member function `assign_phred_to(phred_type const chr, your_type & a)`
 *      of the class `seqan3::custom::alphabet<your_type>`.
 *   2. A free function `assign_phred_to(phred_type const chr, your_type & a)` in the namespace of your type
 *      (or as `friend`).
 *   3. A member function called `assign_phred(phred_type const chr)` (not `assign_phred_to`).
 *
 * Functions are only considered for one of the above cases if they are marked `noexcept` (`constexpr` is not required,
 * but recommended) and if the returned type is `your_type &`.
 *
 * Every writable quality alphabet type must provide one of the above. *Note* that temporaries of `your_type`
 * are handled by this function object and **do not** require an additional overload.
 *
 * ### Customisation point
 *
 * This is a customisation point (see \ref about_customisation). To specify the behaviour for your own alphabet type,
 * simply provide one of the three functions specified above.
 */
inline constexpr auto assign_phred_to = detail::adl_only::assign_phred_to_fn{};
//!\}

} // namespace seqan3

// ============================================================================
// seqan3::quality_alphabet
// ============================================================================

namespace seqan3
{

/*!\interface seqan3::quality_alphabet <>
 * \extends seqan3::alphabet
 * \brief A concept that indicates whether an alphabet represents quality scores.
 * \ingroup quality
 *
 * \details
 *
 * In addition to the requirements for seqan3::alphabet, the
 * quality_alphabet introduces a requirement for conversion functions from and to
 * a Phred score.
 * ### Concepts and doxygen
 *
 * ### Requirements
 *
 *   1. `t` shall model seqan3::alphabet
 *   2. seqan3::to_phred needs to be defined for objects of type `t`
 *
 * See the documentation pages for the respective requirements.
 *
 * ### Related types
 *
 * If a given type `t` models this concept, the following types typically do so, as well:
 *
 *   * `t &`
 *   * `t const`
 *   * `t const &`
 */
//!\cond
template <typename t>
SEQAN3_CONCEPT quality_alphabet = alphabet<t> && requires(t qual)
{
    { seqan3::to_phred(qual) };
};
//!\endcond

// ============================================================================
// seqan3::writable_quality_alphabet
// ============================================================================

/*!\interface seqan3::writable_quality_alphabet <>
 * \extends seqan3::alphabet
 * \brief A concept that indicates whether a writable alphabet represents quality scores.
 * \ingroup quality
 *
 * \details
 *
 * In addition to the requirements for seqan3::writable_alphabet, the seqan3::writable_quality_alphabet
 * introduces the requirements of seqan3::quality_alphabet.
 *
 * ### Requirements
 *
 *   1. `t` shall model seqan3::writable_alphabet
 *   2. `t` shall model seqan3::quality_alphabet
 *   3. seqan3::assign_phred_to needs to be defined for objects of type `t`
 *
 * See the documentation pages for the respective requirements.
 *
 * ### Related types
 *
 * If a given type `t` models this concept, the following types typically do so, as well:
 *
 *   * `t &`
 *
 * `const`-qualified types on the other hand are not assignable.
 */
//!\cond
template <typename t>
SEQAN3_CONCEPT writable_quality_alphabet = writable_alphabet<t> &&
                                         quality_alphabet<t> &&
                                         requires(t v, alphabet_phred_t<t> c)
{
    { seqan3::assign_phred_to(c, v) };
};
//!\endcond

} // namespace seqan3
