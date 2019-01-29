// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides seqan3::align_config::gap.
 * \author Rene Rahn <rene.rahn AT fu-berlin.de>
 * \author Jörg Winkler <j.winkler AT fu-berlin.de>
 */

#pragma once

#include <seqan3/alignment/configuration/detail.hpp>
#include <seqan3/core/metafunction/basic.hpp>
#include <seqan3/alignment/scoring/gap_scheme_concept.hpp>
#include <seqan3/core/algorithm/pipeable_config_element.hpp>

namespace seqan3::align_cfg
{
/*!\brief A configuration element for gaps.
 * \ingroup configuration
 * \tparam gap_scheme_t The type of the underlying gap scheme. Must satisfy the seqan3::gap_scheme_concept.
 */
template <gap_scheme_concept gap_scheme_t>
struct gap : public pipeable_config_element<gap<gap_scheme_t>, gap_scheme_t>
{
    //!\privatesection
    //!\brief Internal id to check for consistent configuration settings.
    static constexpr detail::align_config_id id{detail::align_config_id::gap};
};

/*!\name Type deduction guides
 * \relates seqan3::align_cfg::gap
 * \{
 */
//!\brief Deduces the gap scheme from the constructor argument.
template <gap_scheme_concept scheme_t>
gap(scheme_t) -> gap<remove_cvref_t<scheme_t>>;
//!\}

} // namespace seqan3::align_cfg
