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
 * \author Hannes Hauswedell <hannes.hauswedell AT fu-berlin.de>
 * \brief Provides seqan3::cartesian_composition.
 */

#pragma once

#include <cassert>
#include <utility>

#include <meta/meta.hpp>

#include <seqan3/alphabet/concept.hpp>
#include <seqan3/alphabet/composition/detail.hpp>
#include <seqan3/alphabet/detail/alphabet_base.hpp>
#include <seqan3/alphabet/detail/alphabet_proxy.hpp>
#include <seqan3/alphabet/quality/concept.hpp>
#include <seqan3/core/concept/core_language.hpp>
#include <seqan3/core/concept/tuple.hpp>
#include <seqan3/core/detail/int_types.hpp>
#include <seqan3/core/metafunction/pack.hpp>
#include <seqan3/core/metafunction/transformation_trait_or.hpp>
#include <seqan3/core/tuple_utility.hpp>
#include <seqan3/std/concepts>

namespace seqan3::detail
{

/*!\brief Evaluates to true if one of the components of seqan3::cartesian_composition satisifes a compile-time
 *        predicate.
 * \tparam cartesian_t         A specialisation of seqan3::cartesian_composition.
 * \tparam cartesian_derived_t The CRTP derived type of `cartesian_t`.
 * \tparam fun_t               A template template that takes target_t as argument and exposes an `invoke` member type
 *                             that evaluates some predicate and returns `std::true_type` or `std::false_type`.
 * \tparam target_t            The type you wish to query.
 * \ingroup composition
 *
 * \details
 *
 * To prevent recursive template and/or concept instantiation this call needs to be guarded against many exceptions.
 * See the source file for more details.
 */

// anchor is false
template <typename cartesian_t, typename cartesian_derived_t, template <typename> typename fun_t, typename other_t>
inline bool constexpr one_component_is = false;

//!\cond

// default
template <typename ... cartesian_comps,
          typename cartesian_derived_t,
          template <typename> typename fun_t,
          typename other_t>
inline bool constexpr one_component_is<cartesian_composition<cartesian_derived_t, cartesian_comps...>,
                                       cartesian_derived_t,
                                       fun_t,
                                       other_t> =
    !meta::empty<meta::find_if<meta::list<cartesian_comps...>, fun_t<other_t>>>::value;
    //TODO do without meta

// guard against self
template <typename ... cartesian_comps,
          typename cartesian_derived_t,
          template <typename> typename fun_t>
inline bool constexpr one_component_is<cartesian_composition<cartesian_derived_t, cartesian_comps...>,
                                       cartesian_derived_t,
                                       fun_t,
                                       cartesian_composition<cartesian_derived_t, cartesian_comps...>> = false;

// guard against self (derived)
template <typename ... cartesian_comps,
          typename cartesian_derived_t,
          template <typename> typename fun_t>
inline bool constexpr one_component_is<cartesian_composition<cartesian_derived_t, cartesian_comps...>,
                                       cartesian_derived_t,
                                       fun_t,
                                       cartesian_derived_t> = false;

// guard against types that have conversion operators to derived
template <typename ... cartesian_comps,
          typename cartesian_derived_t,
          template <typename> typename fun_t,
          typename other_t>
    requires convertible_to_by_member_concept<other_t, cartesian_derived_t>
inline bool constexpr one_component_is<cartesian_composition<cartesian_derived_t, cartesian_comps...>,
                                       cartesian_derived_t,
                                       fun_t,
                                       other_t> = false;

// guard against components
template <typename ... cartesian_comps,
          typename cartesian_derived_t,
          template <typename> typename fun_t,
          typename other_t>
    requires type_in_pack_v<other_t, cartesian_comps...>
//     requires meta::in<recursive_cartesian_components<cartesian_composition<cartesian_derived_t, cartesian_comps...>>::type,
//                       other_t>::value
inline bool constexpr one_component_is<cartesian_composition<cartesian_derived_t, cartesian_comps...>,
                                       cartesian_derived_t,
                                       fun_t,
                                       other_t> = false;

// during comparisons, guard against types that could be converted to self (because that is preferred)
// (may not be done during assignment or construction because of recursiveness)
template <typename ... cartesian_comps,
          typename cartesian_derived_t,
          typename other_t>
    requires implicitly_convertible_to_concept<other_t, cartesian_derived_t>
inline bool constexpr one_component_is<cartesian_composition<cartesian_derived_t, cartesian_comps...>,
                                       cartesian_derived_t,
                                       weakly_equality_comparable_with,
                                       other_t> = false;
template <typename ... cartesian_comps,
          typename cartesian_derived_t,
          typename other_t>
    requires implicitly_convertible_to_concept<other_t, cartesian_derived_t>
inline bool constexpr one_component_is<cartesian_composition<cartesian_derived_t, cartesian_comps...>,
                                       cartesian_derived_t,
                                       weakly_ordered_with,
                                       other_t> = false;
//!\endcond

} // namespace seqan3::detail

namespace seqan3
{

// forwards
//!\cond
template <typename t>
decltype(auto) get();

template <size_t i>
decltype(auto) get();
//!\endcond

/*!\brief The CRTP base for a combined alphabet that contains multiple values of different alphabets at the same time.
 * \ingroup composition
 * \implements seqan3::semi_alphabet_concept
 * \implements seqan3::detail::constexpr_semi_alphabet_concept
 * \tparam first_component_type Type of the first letter; must model seqan3::semi_alphabet_concept.
 * \tparam component_types      Types of further letters (up to 4); must model seqan3::semi_alphabet_concept.
 *
 * This data structure is CRTP base class for combined alphabets, where the different
 * alphabet letters exist independently as component_list, similar to a tuple.
 *
 * Short description:
 *   * combines multiple alphabets as independent component_list, similar to a tuple;
 *   * models seqan3::tuple_like_concept, i.e. provides a get interface to its component_list;
 *   * is itself a seqan3::semi_alphabet_concept, but most derived types implement the full seqan3::alphabet_concept;
 *   * its alphabet size is the product of the individual sizes;
 *   * constructible, assignable and comparable with each component type and also all types that
 *     these are constructible/assignable/comparable with;
 *   * explicitly convertible to each of its component types
 *
 * \attention
 * This is a "pure base class", you cannot instantiate it, you can only inherit from it.
 * Most likely you are interested in using one of it's descendants like seqan3::qualified or seqan3::masked.
 * \cond DEV
 * To make a derived class "complete", you should add at least the following:
 *   * .to_char() member
 *   * .assign_char() member
 *   * a type deduction guide
 * \endcond
 *
 * \sa qualified
 * \sa masked
 */
template <typename derived_type,
          typename ...component_types>
//!\cond
    requires (detail::constexpr_semi_alphabet_concept<component_types> && ...)
//!\endcond
class cartesian_composition :
    public alphabet_base<derived_type,
                         (1 * ... * alphabet_size_v<component_types>),
                         void> // no char type, because this is only semi_alphabet
{
private:
    //!\brief The base type of this class.
    using base_t = alphabet_base<derived_type,
                                (1 * ... * alphabet_size_v<component_types>),
                                void>; // no char type, because this is only semi_alphabet

    //!\brief A meta::list The types of each component in the composition
    using component_list = meta::list<component_types...>;

    //!\brief Is set to `true` if the type is contained in the type list.
    template <typename type>
    static constexpr bool is_component =
        meta::in<component_list, type>::value;

    //!\brief Is set to `true` if the type is uniquely contained in the type list.
    template <typename type>
    static constexpr bool is_unique_component =
        is_component<type> &&
        (meta::find_index<component_list, type>::value == meta::reverse_find_index<component_list, type>::value);

    /*!\brief Specialisation of seqan3::alphabet_proxy that updates the rank of the cartesian_composition.
     * \tparam alphabet_type The type of the emulated component.
     * \tparam index         The index of the emulated component.
     */
    template <typename alphabet_type, size_t index>
    class component_proxy : public alphabet_proxy<component_proxy<alphabet_type, index>, alphabet_type>
    {
    private:
        //!\brief The base type.
        using base_t = alphabet_proxy<component_proxy<alphabet_type, index>, alphabet_type>;
        //!\brief Befriend the base type so it can call our #on_update().
        friend base_t;

        //!\brief Store a pointer to the parent object so we can update it.
        cartesian_composition *parent;

        //!\brief The implementation updates the rank in the parent object.
        constexpr void on_update() noexcept
        {
            parent->assign_rank(
                parent->to_rank()
                - parent->template to_component_rank<index>() * cartesian_composition::cummulative_alph_sizes[index]
                + to_rank() * cartesian_composition::cummulative_alph_sizes[index]);
        }

    public:
        //Import from base type:
        using base_t::to_rank;
        using base_t::value_size;
        using base_t::operator=;

        /*!\name Associated types
         * \{
         */
        using typename base_t::rank_type;
        using typename base_t::char_type;
        using typename base_t::phred_type;
        //!\}

        /*!\name Constructors, destructor and assignment
         * \{
         */
        constexpr component_proxy() : base_t{}, parent{} {}
        constexpr component_proxy(component_proxy const &) = default;
        constexpr component_proxy(component_proxy &&) = default;
        constexpr component_proxy & operator=(component_proxy const &) = default;
        constexpr component_proxy & operator=(component_proxy &&) = default;
        ~component_proxy() = default;

        constexpr component_proxy(alphabet_type const l, cartesian_composition & p) :
            base_t{l}, parent{&p}
        {}

        // Does not inherit the base's constructor for alphabet_type so as not to cause ambiguity
        //!\}
    };

    /*!\name Constructors, destructor and assignment
     * \{
     */
    constexpr cartesian_composition() : base_t{} {}
    constexpr cartesian_composition(cartesian_composition const &) = default;
    constexpr cartesian_composition(cartesian_composition &&) = default;
    constexpr cartesian_composition & operator=(cartesian_composition const &) = default;
    constexpr cartesian_composition & operator=(cartesian_composition &&) = default;
    ~cartesian_composition() = default;

    using base_t::base_t;
    //!\}

    //!\brief befriend the derived type so that it can instantiate
    //!\sa https://isocpp.org/blog/2017/04/quick-q-prevent-user-from-derive-from-incorrect-crtp-base
    friend derived_type;

public:
    // Import from base:
    using base_t::value_size;
    using typename base_t::rank_type;
    using base_t::to_rank;
    using base_t::assign_rank;

    //!\brief Export this type's components in a visible manner.
    //!\private
    using seqan3_cartesian_components = component_list;
    //!\brief Export this type's components and possibly the components' components in a visible manner.
    //!\private
    using seqan3_recursive_cartesian_components =
        meta::concat<component_list,
                     detail::transformation_trait_or_t<detail::recursive_cartesian_components<component_types>,
                                                       meta::list<>>...>;

    /*!\name Constructors, destructor and assignment
     * \{
     * \attention Please do not directly use the CRTP base class. The functions
     *            are only public for the usage in their derived classes (e.g.
     *            seqan3::qualified, seqan3::masked, seqan3::structure_rna and
     *            seqan3::structure_aa).
     */
    //!\brief Construction from initialiser-list.
    constexpr cartesian_composition(component_types ... components)
    {
        assign_rank(rank_sum_helper(components..., std::make_index_sequence<sizeof...(component_types)>{}));
    }

    /*!\brief Construction via a value of one of the components.
     * \tparam component_type Must be one uniquely contained in the type
                              list of the composition.
     * \param  alph           The value of a component that should be assigned.
     *
     * Note: Since the cartesian_composition is a CRTP base class, we show the working examples
     * with one of its derived classes (seqan3::qualified).
     * \snippet test/snippet/alphabet/composition/cartesian_composition.cpp value_construction
     */
    template <typename component_type>
    //!\cond
        requires is_unique_component<component_type>
    //!\endcond
    constexpr explicit cartesian_composition(component_type const alph) : cartesian_composition{}
    {
        get<component_type>(*this) = alph;
    }

    /*!\brief Construction via a value of a subtype that is assignable to one of the components.
     * \tparam indirect_component_type Type that models the seqan3::is_assignable_concept for
     *                                 one of the component types.
     * \param  alph                    The value that should be assigned.
     *
     * Note that the value will be assigned to the **FIRST** type T that fulfils
     * the `assignable_concept<T, indirect_component_type>`, regardless if other types are also
     * fit for assignment.
     *
     * Note: Since the cartesian_composition is a CRTP base class, we show the working examples
     * with one of its derived classes (seqan3::qualified).
     * \snippet test/snippet/alphabet/composition/cartesian_composition.cpp subtype_construction
     */
    template <typename indirect_component_type>
    //!\cond
       requires detail::one_component_is<cartesian_composition, derived_type, detail::implicitly_convertible_from, indirect_component_type>
    //!\endcond
    constexpr explicit cartesian_composition(indirect_component_type const alph) : cartesian_composition{}
    {
       using component_type = meta::front<meta::find_if<component_list, detail::implicitly_convertible_from<indirect_component_type>>>;
       component_type tmp(alph); // delegate construction
       get<component_type>(*this) = tmp;
    }

    //!\cond
    template <typename indirect_component_type>
       requires !detail::one_component_is<cartesian_composition, derived_type, detail::implicitly_convertible_from, indirect_component_type> &&
                 detail::one_component_is<cartesian_composition, derived_type, detail::constructible_from, indirect_component_type>
    constexpr explicit cartesian_composition(indirect_component_type const alph) : cartesian_composition{}
    {
       using component_type = meta::front<meta::find_if<component_list, detail::constructible_from<indirect_component_type>>>;
       component_type tmp(alph); // delegate construction
       get<component_type>(*this) = tmp;
    }
    //!\endcond

    /*!\brief Assignment via a value of one of the components.
     * \tparam component_type One of the component types. Must be uniquely
     *                        contained in the type list of the composition.
     * \param  alph           The value of a component that should be assigned.
     *
     * Note: Since the cartesian_composition is a CRTP base class, we show the working examples
     * with one of its derived classes (seqan3::qualified).
     * \snippet test/snippet/alphabet/composition/cartesian_composition.cpp value_assignment
     */
    template <typename component_type>
    //!\cond
        requires is_unique_component<component_type>
    //!\endcond
    constexpr derived_type & operator=(component_type const alph)
    {
        get<component_type>(*this) = alph;
        return static_cast<derived_type &>(*this);
    }

    /*!\brief Assignment via a value of a subtype that is assignable to one of the components.
     * \tparam indirect_component_type Type that models the seqan3::is_assignable_concept for
     *                                 one of the component types.
     * \param  alph                    The value of a component that should be assigned.
     *
     * Note: Since the cartesian_composition is a CRTP base class, we show the working examples
     * with one of its derived classes (seqan3::qualified).
     * \snippet test/snippet/alphabet/composition/cartesian_composition.cpp subtype_assignment
     */
    template <typename indirect_component_type>
    //!\cond
        requires detail::one_component_is<cartesian_composition, derived_type, detail::assignable_from, indirect_component_type>
    //!\endcond
    constexpr derived_type & operator=(indirect_component_type const alph)
    {
        using component_type = meta::front<meta::find_if<component_list, detail::assignable_from<indirect_component_type>>>;
        get<component_type>(*this) = alph; // delegate assignment
        return static_cast<derived_type &>(*this);
    }
    //!\cond
    // If not assignable but implicit convertible, convert first and assign afterwards
    template <typename indirect_component_type>
    //!\cond
        requires !detail::one_component_is<cartesian_composition, derived_type, detail::assignable_from, indirect_component_type> &&
                 detail::one_component_is<cartesian_composition, derived_type, detail::implicitly_convertible_from, indirect_component_type>
    //!\endcond
    constexpr derived_type & operator=(indirect_component_type const alph)
    {
        using component_type = meta::front<meta::find_if<component_list, detail::implicitly_convertible_from<indirect_component_type>>>;
        component_type tmp(alph);
        get<component_type>(*this) = tmp;
        return static_cast<derived_type &>(*this);
    }
    //!\endcond
    //!\}

    /*!\name Read functions
     * \brief All read operations are constant complexity.
     * \{
     */
    /*!\brief Tuple-like access to the contained components.
     * \tparam index Return the i-th element.
     * \returns A proxy to the contained element that models the same alphabets concepts and supports assignment.
     */
    template <size_t index>
    friend constexpr auto get(cartesian_composition & l)
    {
        static_assert(index < sizeof...(component_types), "Index out of range.");

        using t = meta::at_c<component_list, index>;
        t val{};

        using seqan3::assign_rank;
        assign_rank(val, l.to_component_rank<index>());

        return component_proxy<t, index>{val, l};
    }

    /*!\copybrief get
     * \tparam type Return the element of specified type; only available if the type is unique in the set of components.
     * \returns A proxy to the contained element that models the same alphabets concepts and supports assignment.
     */
    template <typename type>
    friend constexpr auto get(cartesian_composition & l)
    //!\cond
        requires is_unique_component<type>
    //!\endcond
    {
        return get<meta::find_index<component_list, type>::value>(l);
    }

    /*!\copybrief get
     * \tparam index Return the i-th element.
     * \returns A copy of the contained element.
     */
    template <size_t index>
    friend constexpr auto get(cartesian_composition const & l)
    {
        static_assert(index < sizeof...(component_types), "Index out of range.");

        using t = meta::at_c<component_list, index>;
        t val{};

        using seqan3::assign_rank;
        assign_rank(val, l.to_component_rank<index>());

        return val;
    }

    /*!\copybrief get
     * \tparam type Return the element of specified type; only available if the type is unique in the set of components.
     * \returns A copy of the contained element.
     */
    template <typename type>
    friend constexpr type get(cartesian_composition const & l)
    //!\cond
        requires is_unique_component<type>
    //!\endcond
    {
        return get<meta::find_index<component_list, type>::value>(l);
    }

    /*!\brief Implicit cast to a single letter. Works only if the type is unique in the type list.
     */
    template <typename type>
    constexpr operator type() const
    //!\cond
        requires is_unique_component<type>
    //!\endcond
    {
        return get<type>(*this);
    }
    //!\}

    /*!\name Comparison operators (against indirect component_list)
     * \brief `*this` is cast to the component type before comparison. (These overloads enable comparison for all types
     *        that a component type is comparable with).
     * \{
     */
    template <typename indirect_component_type>
    constexpr bool operator==(indirect_component_type const & rhs) const noexcept
    //!\cond
        requires detail::one_component_is<cartesian_composition, derived_type, detail::weakly_equality_comparable_with, indirect_component_type>
    //!\endcond
    {
        using component_type = meta::front<meta::find_if<component_list, detail::weakly_equality_comparable_with<indirect_component_type>>>;
        return get<component_type>(*this) == rhs;
    }

    template <typename indirect_component_type>
    constexpr bool operator!=(indirect_component_type const & rhs) const noexcept
    //!\cond
        requires detail::one_component_is<cartesian_composition, derived_type, detail::weakly_equality_comparable_with, indirect_component_type>
    //!\endcond
    {
        using component_type = meta::front<meta::find_if<component_list, detail::weakly_equality_comparable_with<indirect_component_type>>>;
        return get<component_type>(*this) != rhs;
    }

    template <typename indirect_component_type>
    constexpr bool operator<(indirect_component_type const & rhs) const noexcept
    //!\cond
        requires detail::one_component_is<cartesian_composition, derived_type, detail::weakly_ordered_with, indirect_component_type>
    //!\endcond
    {
        using component_type = meta::front<meta::find_if<component_list, detail::weakly_ordered_with<indirect_component_type>>>;
        return get<component_type>(*this) < rhs;
    }

    template <typename indirect_component_type>
    constexpr bool operator>(indirect_component_type const & rhs) const noexcept
    //!\cond
        requires detail::one_component_is<cartesian_composition, derived_type, detail::weakly_ordered_with, indirect_component_type>
    //!\endcond
    {
        using component_type = meta::front<meta::find_if<component_list, detail::weakly_ordered_with<indirect_component_type>>>;
        return get<component_type>(*this) > rhs;
    }

    template <typename indirect_component_type>
    constexpr bool operator<=(indirect_component_type const & rhs) const noexcept
    //!\cond
        requires detail::one_component_is<cartesian_composition, derived_type, detail::weakly_ordered_with, indirect_component_type>
    //!\endcond
    {
        using component_type = meta::front<meta::find_if<component_list, detail::weakly_ordered_with<indirect_component_type>>>;
        return get<component_type>(*this) <= rhs;
    }

    template <typename indirect_component_type>
    constexpr bool operator>=(indirect_component_type const & rhs) const noexcept
    //!\cond
        requires detail::one_component_is<cartesian_composition, derived_type, detail::weakly_ordered_with, indirect_component_type>
    //!\endcond
    {
        using component_type = meta::front<meta::find_if<component_list, detail::weakly_ordered_with<indirect_component_type>>>;
        return get<component_type>(*this) >= rhs;
    }
    //!\}

private:
    //!\brief Return the rank of the i-th component.
    template <size_t index>
    constexpr rank_type to_component_rank() const
    {
        return (to_rank() / cummulative_alph_sizes[index]) % alphabet_size_v<meta::at_c<component_list, index>>;
    }

    //!\brief the cumulative alphabet size products are cached
    static constexpr std::array<rank_type, component_list::size()> cummulative_alph_sizes
    {
        [] () constexpr
        {
            // create array (1, |sigma1|, |sigma1|*|sigma2|,  ... ,  |sigma1|*...*|sigmaN|)
            std::array<rank_type, component_list::size() + 1> ret{};
            ret[0] = 1;
            size_t count = 1;
            meta::for_each(meta::reverse<component_list>{}, [&] (auto && alph) constexpr
            {
                ret[count] = static_cast<rank_type>(alphabet_size_v<std::decay_t<decltype(alph)>> * ret[count - 1]);
                ++count;
            });

            // reverse and strip one: (|sigma1|*...*|sigmaN-1|, ... |sigma1|*|sigma2|, |sigma1|, 1)
            // reverse order guarantees that the first alphabet is the most significant rank contributer
            // resulting in element-wise alphabetical ordering on comparison
            std::array<rank_type, component_list::size()> ret2{};
            for (size_t i = 0; i < component_list::size(); ++i)
                ret2[i] = ret[component_list::size() - i - 1];

            return ret2;
        }()
    };

    //!\brief For the given components, compute the combined rank.
    template <std::size_t ...idx>
    static constexpr rank_type rank_sum_helper(component_types ... components, std::index_sequence<idx...> const &)
    {
        using seqan3::to_rank;
        return ((to_rank(components) * cummulative_alph_sizes[idx]) + ...);
    }
};

/*!\name Comparison operators
 * \relates seqan3::cartesian_composition
 * \{
 * \brief Free function comparison operators that forward to member operators (for types != self).
 */
template <typename indirect_component_type, typename derived_type, typename ...component_types>
//!\cond
    requires detail::weakly_equality_comparable_by_members_with_concept<derived_type, indirect_component_type> &&
             !detail::weakly_equality_comparable_by_members_with_concept<indirect_component_type, derived_type>
//!\endcond
constexpr bool operator==(indirect_component_type const & lhs,
                          cartesian_composition<derived_type, component_types...> const & rhs)
{
    return rhs == lhs;
}

template <typename indirect_component_type, typename derived_type, typename ...indirect_component_types>
//!\cond
    requires detail::weakly_equality_comparable_by_members_with_concept<derived_type, indirect_component_type> &&
             !detail::weakly_equality_comparable_by_members_with_concept<indirect_component_type, derived_type>
//!\endcond
constexpr bool operator!=(indirect_component_type const & lhs,
                          cartesian_composition<derived_type, indirect_component_types...> const & rhs)
{
    return rhs != lhs;
}

template <typename indirect_component_type, typename derived_type, typename ...indirect_component_types>
//!\cond
    requires detail::weakly_ordered_by_members_with_concept<derived_type, indirect_component_type> &&
             !detail::weakly_ordered_by_members_with_concept<indirect_component_type, derived_type>
//!\endcond
constexpr bool operator<(indirect_component_type const & lhs,
                         cartesian_composition<derived_type, indirect_component_types...> const & rhs)
{
    return rhs > lhs;
}

template <typename indirect_component_type, typename derived_type, typename ...indirect_component_types>
//!\cond
    requires detail::weakly_ordered_by_members_with_concept<derived_type, indirect_component_type> &&
             !detail::weakly_ordered_by_members_with_concept<indirect_component_type, derived_type>
//!\endcond
constexpr bool operator>(indirect_component_type const & lhs,
                         cartesian_composition<derived_type, indirect_component_types...> const & rhs)
{
    return rhs < lhs;
}

template <typename indirect_component_type, typename derived_type, typename ...indirect_component_types>
//!\cond
    requires detail::weakly_ordered_by_members_with_concept<derived_type, indirect_component_type> &&
             !detail::weakly_ordered_by_members_with_concept<indirect_component_type, derived_type>
//!\endcond
constexpr bool operator<=(indirect_component_type const & lhs,
                          cartesian_composition<derived_type, indirect_component_types...> const & rhs)
{
    return rhs >= lhs;
}

template <typename indirect_component_type, typename derived_type, typename ...indirect_component_types>
//!\cond
    requires detail::weakly_ordered_by_members_with_concept<derived_type, indirect_component_type> &&
             !detail::weakly_ordered_by_members_with_concept<indirect_component_type, derived_type>
//!\endcond
constexpr bool operator>=(indirect_component_type const & lhs,
                          cartesian_composition<derived_type, indirect_component_types...> const & rhs)
{
    return rhs <= lhs;
}
//!\}

} // namespace seqan3

namespace std
{

//!\brief Obtains the type of the specified element.
//!\relates seqan3::pod_tuple
template <std::size_t i, seqan3::detail::cartesian_composition_concept tuple_t>
struct tuple_element<i, tuple_t>
{
    //!\brief Element type.
    using type = meta::at_c<typename tuple_t::seqan3_cartesian_components, i>;
};

//!\brief Provides access to the number of elements in a tuple as a compile-time constant expression.
//!\relates seqan3::pod_tuple
template <seqan3::detail::cartesian_composition_concept tuple_t>
struct tuple_size<tuple_t> :
    public std::integral_constant<size_t, tuple_t::seqan3_cartesian_components::size()>
{};

} // namespace std
