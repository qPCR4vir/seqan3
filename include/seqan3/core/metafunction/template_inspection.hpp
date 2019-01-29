// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \author Hannes Hauswedell <hannes.hauswedell AT fu-berlin.de>
 * \brief Provides seqan3::type_list and auxiliary metafunctions.
 */

#pragma once

#include <meta/meta.hpp>

#include <seqan3/core/metafunction/transformation_trait_or.hpp>

#include <seqan3/std/concepts>

namespace seqan3::detail
{

// ----------------------------------------------------------------------------
// transfer_template_args_onto
// ----------------------------------------------------------------------------

//!\cond
template <typename source_type, template <typename ...> typename target_template>
struct transfer_template_args_onto
{};
//!\endcond

/*!\brief Type metafunction that extracts a type template's **type** arguments and specialises another template
 * with them [metafunction definition].
 * \tparam source_template   The source type; must be a specialisation of a template.
 * \tparam target_template   The type template you wish to specialise.
 * \tparam source_arg_types  The **type** arguments to the source_template (deduced implicitly).
 * \ingroup metafunction
 * \see seqan3::detail::transfer_template_vargs_onto
 *
 * \details
 *
 * Among other use cases, it enables using the types contained in a seqan3::type_list to specialise another type
 * template.
 *
 * A metafunction shortcut is also defined: seqan3::detail::transfer_template_args_onto_t
 *
 * This metafunction works for templates that have **only type-arguments**. See
 * seqan3::detail::transfer_template_vargs_onto for a metafunction that transfers non-type arguments. There is
 * no metafunction that can handle a combination of type and non-type arguments.
 * If the `source_type` is a not a template class, e.g. an `int`, the member type `type` is not defined.
 *
 * ### Example
 *
 * \snippet test/snippet/core/metafunction/template_inspection.cpp usage
 */

template <template <typename ...> typename source_template,
          template <typename ...> typename target_template,
          typename ... source_arg_types>
//!\cond
    requires requires ()
    {
        std::declval<target_template<source_arg_types...>>();
    }
//!\endcond
struct transfer_template_args_onto<source_template<source_arg_types...>, target_template>
{
    //!\brief The return type: the target type specialised by the unpacked types in the list.
    using type = target_template<source_arg_types...>;
};

/*!\brief Type metafunction shortcut for seqan3::detail::transfer_template_args_onto.
 * \ingroup metafunction
 * \see seqan3::detail::transfer_template_args_onto
 */
template <typename source_type, template <typename ...> typename target_template>
using transfer_template_args_onto_t = typename transfer_template_args_onto<source_type, target_template>::type;

// ----------------------------------------------------------------------------
// transfer_template_vargs_onto
// ----------------------------------------------------------------------------

//!\cond
template <typename source_type, template <auto ...> typename target_template>
struct transfer_template_vargs_onto
{};
//!\endcond

/*!\brief Type metafunction that extracts a type template's **non-type** arguments and specialises another template
 * with them [metafunction definition].
 * \tparam source_template   The source type; must be a specialisation of a template.
 * \tparam target_template   The type template you wish to specialise.
 * \tparam source_varg_types The **non-type** arguments to the source_template (deduced implicitly).
 * \ingroup metafunction
 * \see seqan3::detail::transfer_template_vargs_onto
 *
 * \details
 *
 * A metafunction shortcut is also defined: seqan3::detail::transfer_template_vargs_onto_t
 *
 * This metafunction works for templates that have **only non-type-arguments**. See
 * seqan3::detail::transfer_template_args_onto for a metafunction that transfers type arguments. There is
 * no metafunction that can handle a combination of type and non-type arguments.
 * If the `source_type` is a not a template class, e.g. an `int`, the member type `type` is not defined.
 */
template <template <auto ...> typename source_template,
          template <auto ...> typename target_template,
          auto ... source_varg_types>
//!\cond
    requires requires ()
    {
        std::declval<target_template<source_varg_types...>>();
    }
//!\endcond
struct transfer_template_vargs_onto<source_template<source_varg_types...>, target_template>
{
    //!\brief The return type: the target type specialised by the unpacked types in the list.
    using type = target_template<source_varg_types...>;
};

/*!\brief Type metafunction shortcut for seqan3::detail::transfer_template_vargs_onto.
 * \ingroup metafunction
 * \see seqan3::detail::transfer_template_vargs_onto
 */
template <typename source_type, template <auto ...> typename target_template>
using transfer_template_vargs_onto_t = typename transfer_template_vargs_onto<source_type, target_template>::type;

// ----------------------------------------------------------------------------
// is_type_specialisation_of_v
// ----------------------------------------------------------------------------

/*!\brief Value metafunction that returns whether a source_type is a specialisation of another template.
 * \tparam source_type      The source type.
 * \tparam target_template  The type template you wish to compare against (must take only types as template arguments).
 * \ingroup metafunction
 * \see seqan3::detail::is_value_specialisation_of
 * \see seqan3::detail::is_type_specialisation_of_v
 *
 * \details
 *
 * ### Example
 *
 * \snippet test/snippet/core/metafunction/template_inspection.cpp usage_2
 */
template <typename source_t, template <typename ...> typename target_template>
struct is_type_specialisation_of : public std::false_type
{};

template <typename source_t, template <typename ...> typename target_template>
//!\cond
    requires !std::Same<transformation_trait_or_t<transfer_template_args_onto<source_t, target_template>, void>,
                        void>
//!\endcond
struct is_type_specialisation_of<source_t, target_template> :
        std::is_same<source_t, transfer_template_args_onto_t<source_t, target_template>>
{};

/*!\brief Helper variable template for seqan3::detail::is_type_specialisation_of.
 * \tparam source_type      The source type.
 * \tparam target_template  The type template you wish to compare against (must take only types as template arguments).
 * \ingroup metafunction
 */
template <typename source_t, template <typename ...> typename target_template>
inline constexpr bool is_type_specialisation_of_v = is_type_specialisation_of<source_t, target_template>::value;

// ----------------------------------------------------------------------------
// is_value_specialisation_of_v
// ----------------------------------------------------------------------------

//!\cond
template <typename source_t, template <auto ...> typename target_template>
struct is_value_specialisation_of : std::false_type
{};
//!\endcond

/*!\brief Value metafunction that returns whether a source_type is a specialisation of another template.
 * \tparam source_type      The source type.
 * \tparam target_template  The type template you wish to compare against (must take only non-types as template
 * arguments).
 * \ingroup metafunction
 * \see seqan3::detail::is_type_specialisation_of
 * \see seqan3::detail::is_value_specialisation_of_v
 */
template <typename source_t, template <auto ...> typename target_template>
//!\cond
    requires !std::Same<transformation_trait_or_t<transfer_template_vargs_onto<source_t, target_template>, void>,
                        void>
//!\endcond
struct is_value_specialisation_of<source_t, target_template> :
    std::is_same<source_t, transfer_template_vargs_onto_t<source_t, target_template>>
{};

/*!\brief Helper variable template for seqan3::detail::is_value_specialisation_of.
 * \tparam source_type      The source type.
 * \tparam target_template  The type template you wish to compare against (must take only types as template arguments).
 * \ingroup metafunction
 */
template <typename source_t, template <auto ...> typename target_template>
inline constexpr bool is_value_specialisation_of_v = is_value_specialisation_of<source_t, target_template>::value;

} // namespace seqan3::detail
