// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides the configuration for returning positions in the text.
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

//!\brief Type for the "index_cursor" value for the configuration element "output".
struct search_output_index_cursor {};
//!\brief Type for the "text_position" value for the configuration element "output".
struct search_output_text_position {};

} // namespace seqan3::detail

namespace seqan3::search_cfg
{

//!\brief Configuration element to receive all hits within the error bounds.
inline detail::search_output_index_cursor constexpr index_cursor;
//!\brief Configuration element to receive all hits within the lowest number of errors.
inline detail::search_output_text_position constexpr text_position;

/*!\brief Configuration element to determine the output type of hits.
 * \ingroup search_configuration
 */
template <typename output_t>
//!\cond
    requires std::Same<remove_cvref_t<output_t>, detail::search_output_text_position> ||
             std::Same<remove_cvref_t<output_t>, detail::search_output_index_cursor>
//!\endcond
struct output : public pipeable_config_element<output<output_t>, output_t>
{
    //!\privatesection
    //!\brief Internal id to check for consistent configuration settings.
    static constexpr detail::search_config_id id{detail::search_config_id::output};
};

/*!\name Type deduction guides
 * \relates seqan3::search_cfg::output
 * \{
 */

//!\brief Deduces search output type from constructor argument.
template <typename output_t>
output(output_t) -> output<remove_cvref_t<output_t>>;
//!\}

} // namespace seqan3::search_cfg

//!\}
