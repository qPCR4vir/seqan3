// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides the mode configuration to define the search modes "all", "all_best", "best" and "strata".
 * \author Christopher Pockrandt <christopher.pockrandt AT fu-berlin.de>
 * \author Rene Rahn <rene.rahn AT fu-berlin.de>
 */

#pragma once

#include <seqan3/core/algorithm/pipeable_config_element.hpp>
#include <seqan3/core/detail/strong_type.hpp>
#include <seqan3/core/metafunction/basic.hpp>
#include <seqan3/search/algorithm/configuration/detail.hpp>

/*!\addtogroup search
 * \{
 */

namespace seqan3::detail
{

//!\brief Type for the "all" value for the configuration element "mode".
struct search_mode_all {};
//!\brief Type for the "all_best" value for the configuration element "mode".
struct search_mode_all_best {};
//!\brief Type for the "best" value for the configuration element "mode".
struct search_mode_best {};

} // namespace seqan3::detail

namespace seqan3::search_cfg
{

//!\brief Configuration element to receive all hits within the error bounds.
inline detail::search_mode_all constexpr all;
//!\brief Configuration element to receive all hits within the lowest number of errors.
inline detail::search_mode_all_best constexpr all_best;
//!\brief Configuration element to receive one best hit (with the lowest number of errors).
inline detail::search_mode_best constexpr best;

/*!\brief Configuration element to receive all hits with the best number of errors plus the strata value.
 *        A strong type of underlying type `uint8_t` that represents the number or errors for strata.
 *        All hits are found with the fewest numbererrors plus 'value'.
 * \tparam value_t The underlying type
 * \ingroup search_configuration
 */
struct strata : detail::strong_type<uint8_t, strata, detail::strong_type_skill::convert>
{
    using detail::strong_type<uint8_t, strata, detail::strong_type_skill::convert>::strong_type;
};

/*!\brief Configuration element to determine the search mode.
 * \ingroup search_configuration
 */
template <typename mode_t>
//!\cond
    requires std::Same<remove_cvref_t<mode_t>, detail::search_mode_all> ||
             std::Same<remove_cvref_t<mode_t>, detail::search_mode_all_best> ||
             std::Same<remove_cvref_t<mode_t>, detail::search_mode_best> ||
             std::Same<remove_cvref_t<mode_t>, strata>
//!\endcond
struct mode : public pipeable_config_element<mode<mode_t>, mode_t>
{
    //!\privatesection
    //!\brief Internal id to check for consistent configuration settings.
    static constexpr detail::search_config_id id{detail::search_config_id::mode};
};

/*!\name Type deduction guides
 * \relates seqan3::search_cfg::mode
 * \{
 */

//!\brief Deduces search mode type from constructor argument.
template <typename mode_t>
mode(mode_t) -> mode<remove_cvref_t<mode_t>>;
//!\}
} // namespace seqan3::search_cfg

//!\}
