// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \author Marcel Ehrhardt <marcel.ehrhardt AT fu-berlin.de>
 * \brief Provides seqan3::detail::transformation_trait_or.
 */

#pragma once

#include <type_traits>

#include <meta/meta.hpp>

#include <seqan3/core/platform.hpp>
#include <seqan3/std/type_traits>

namespace seqan3::detail
{

/*!\brief This gives a fallback type if *type_t::type* is not defined.
 * \ingroup metafunction
 * \tparam type_t    The type to use if *type_t::type* is defined.
 * \tparam default_t The type to use otherwise.
 *
 * \details
 *
 * Gives *type_t* back if *T::type* is a member type, otherwise *struct{using type = default_t}*.
 *
 * \include test/snippet/core/metafunction/transformation_trait_or.cpp
 *
 * \attention This might get removed if one of our used libraries offers the same
 * functionality.
 *
 * \par Helper types
 *   seqan3::detail::transformation_trait_or_t as a shorthand for *seqan3::detail::transformation_trait_or::type*
 */
template <typename type_t, typename default_t>
using transformation_trait_or = std::conditional_t<meta::is_trait<type_t>::value,   // check if type_t::type exists
                                                   type_t,                          // if yes, return type_t
                                                   std::type_identity<default_t>>;  // else return default_t as trait

/*!\brief Helper type of seqan3::detail::transformation_trait_or
 * \ingroup metafunction
 * \relates transformation_trait_or
 */
template <typename type_t, typename default_t>
using transformation_trait_or_t = typename transformation_trait_or<type_t, default_t>::type;

} // namespace seqan3::detail
