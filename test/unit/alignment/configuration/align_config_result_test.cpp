// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <type_traits>

#include <seqan3/alignment/configuration/align_config_result.hpp>
#include <seqan3/core/algorithm/configuration.hpp>

using namespace seqan3;

template <typename test_t>
struct align_cfg_result_test : public ::testing::Test
{

};

using test_types = ::testing::Types<detail::with_score_type,
                                    detail::with_end_position_type,
                                    detail::with_begin_position_type,
                                    detail::with_trace_type>;

TYPED_TEST_CASE(align_cfg_result_test, test_types);

TEST(align_config_max_error, config_element_concept)
{
    EXPECT_TRUE((detail::config_element_concept<align_cfg::result<detail::with_score_type>>));
}

template <typename type>
auto type_to_variable()
{
    using namespace seqan3::align_cfg;

    if constexpr (std::is_same_v<type, detail::with_score_type>)
    {
        return with_score;
    }
    else if constexpr (std::is_same_v<type, detail::with_end_position_type>)
    {
        return with_end_position;
    }
    else if constexpr (std::is_same_v<type, detail::with_begin_position_type>)
    {
        return with_begin_position;
    }
    else
    {
        return with_trace;
    }
}

TYPED_TEST(align_cfg_result_test, configuration)
{
    {
        align_cfg::result elem{TypeParam{}};
        configuration cfg{elem};
        EXPECT_TRUE((std::is_same_v<std::remove_reference_t<decltype(get<align_cfg::result>(cfg).value)>,
                                    TypeParam>));
    }

    {
        configuration cfg{align_cfg::result{type_to_variable<TypeParam>()}};
        EXPECT_TRUE((std::is_same_v<std::remove_reference_t<decltype(get<align_cfg::result>(cfg).value)>,
                                    TypeParam>));
    }
}
