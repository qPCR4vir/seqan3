// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides seqan3::align_cfg::detail::debug.
 * \author Rene Rahn <rene.rahn AT fu-berlin.de>
 */

#pragma once

#include <seqan3/alignment/configuration/detail.hpp>
#include <seqan3/core/configuration/detail/configuration_element_debug_mode.hpp>
#include <seqan3/core/configuration/pipeable_config_element.hpp>

namespace seqan3::align_cfg::detail
{
/*!\brief Configuration element for debugging the alignment algorithm.
 * \ingroup alignment_configuration
 *
 * \details
 *
 * Using this configuration allows to output the alignment matrices from the DP algorithm using the
 * returned seqan3::alignment_result.
 * The score matrix is always accessible, while the trace matrix can only be computed if an alignment was
 * requested via the seqan3::align_cfg::output_alignment configuration.
 *
 * \note This configuration is only useful for debugging purposes as it can have a significant impact on the
 *       performance.
 */
using debug = seqan3::detail::debug_mode<
    std::integral_constant<seqan3::detail::align_config_id, seqan3::detail::align_config_id::debug>>;
} // namespace seqan3::align_cfg::detail
