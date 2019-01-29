// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <seqan3/alignment/configuration/align_config_gap.hpp>
#include <seqan3/alignment/scoring/gap_scheme.hpp>
#include <seqan3/core/algorithm/configuration.hpp>
#include <seqan3/std/concepts>

using namespace seqan3;

TEST(align_config_gap, config_element_concept)
{
    EXPECT_TRUE((detail::config_element_concept<align_cfg::gap<gap_scheme<>>>));
}

TEST(align_config_gap, configuration)
{
    {
        align_cfg::gap elem{gap_scheme<>{}};
        configuration cfg{elem};
        EXPECT_EQ((get<align_cfg::gap>(cfg).value.get_gap_score()), -1);
    }

    {
        configuration cfg{align_cfg::gap{gap_scheme<>{}}};
        EXPECT_EQ((get<align_cfg::gap>(cfg).value.get_gap_open_score()), 0);
    }
}
