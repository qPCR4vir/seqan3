// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <meta/meta.hpp>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <seqan3/alignment/pairwise/align_result.hpp>
#include <seqan3/alphabet/gap/gapped.hpp>
#include <seqan3/alphabet/nucleotide/all.hpp>
#include <seqan3/core/metafunction/template_inspection.hpp>
#include <seqan3/range/view/persist.hpp>
#include <seqan3/range/view/to_char.hpp>

using namespace seqan3;

using aligned_seq_type = std::vector<gapped<dna4>>;

template <typename T>
struct align_result_test : public ::testing::Test
{};

using align_result_test_types = ::testing::Types
      <detail::align_result_value_type<uint32_t, int32_t, std::pair<size_t, size_t>, std::pair<size_t, size_t>,
                                       std::pair<aligned_seq_type, aligned_seq_type>>,
       detail::align_result_value_type<uint32_t, int32_t, std::pair<size_t, size_t>, std::pair<size_t, size_t>,
                                       std::tuple<aligned_seq_type, aligned_seq_type>>,
       detail::align_result_value_type<uint32_t, int32_t, std::pair<size_t, size_t>, std::pair<size_t, size_t>,
                                       std::vector<aligned_seq_type>>,
       detail::align_result_value_type<uint32_t, float,   std::pair<size_t, size_t>, std::pair<size_t, size_t>,
                                       std::pair<aligned_seq_type, aligned_seq_type>>,
       detail::align_result_value_type<uint32_t, float,   std::pair<size_t, size_t>, std::pair<size_t, size_t>,
                                       std::tuple<aligned_seq_type, aligned_seq_type>>,
       detail::align_result_value_type<uint32_t, float,   std::pair<size_t, size_t>, std::pair<size_t, size_t>,
                                       std::vector<aligned_seq_type>>>;

TYPED_TEST_CASE(align_result_test, align_result_test_types);

TYPED_TEST(align_result_test, type_specialisation)
{
    EXPECT_TRUE((detail::is_type_specialisation_of_v<TypeParam, detail::align_result_value_type>));
}

TYPED_TEST(align_result_test, constructor)
{
    EXPECT_TRUE((std::is_default_constructible_v<align_result<TypeParam>>));
    EXPECT_TRUE((std::is_copy_constructible_v<align_result<TypeParam>>));
    EXPECT_TRUE((std::is_move_constructible_v<align_result<TypeParam>>));
    EXPECT_TRUE((std::is_copy_assignable_v<align_result<TypeParam>>));
    EXPECT_TRUE((std::is_move_assignable_v<align_result<TypeParam>>));
    EXPECT_TRUE((std::is_destructible_v<align_result<TypeParam>>));
}

TYPED_TEST(align_result_test, get_id)
{
    using id_t = decltype(std::declval<TypeParam>().id);
    aligned_seq_type seq{'A'_dna4, 'T'_dna4, gap{}, 'C'_dna4, gap{}, gap{}, 'A'_dna4};

    {
        align_result<TypeParam> tmp{TypeParam{1u, 0, {10ul, 10ul}, {0ul, 0ul}, {seq, seq}}};
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_id()), id_t>));
        EXPECT_TRUE((std::is_same_v<decltype(std::move(tmp).get_id()), id_t>));
        EXPECT_EQ(tmp.get_id(), 1u);
    }

    {
        align_result<TypeParam> const tmp{TypeParam{1u, 0, {10ul, 10ul}, {0ul, 0ul}, {seq, seq}}};
        EXPECT_EQ(tmp.get_id(), 1u);
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_id()), id_t>));
        EXPECT_TRUE((std::is_same_v<decltype(std::move(tmp).get_id()), id_t>));
    }
}

TYPED_TEST(align_result_test, get_score)
{
    using score_t = decltype(std::declval<TypeParam>().score);
    aligned_seq_type seq{'A'_dna4, 'T'_dna4, gap{}, 'C'_dna4, gap{}, gap{}, 'A'_dna4};

    {
        align_result<TypeParam> tmp{TypeParam{1u, 0, {10ul, 10ul}, {0ul, 0ul}, {seq, seq}}};
        EXPECT_EQ(tmp.get_score(), 0);
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_score()), score_t>));
        EXPECT_TRUE((std::is_same_v<decltype(std::move(tmp).get_score()), score_t>));
    }

    {
        align_result<TypeParam> const tmp{TypeParam{1u, 0, {10ul, 10ul}, {0ul, 0ul}, {seq, seq}}};
        EXPECT_EQ(tmp.get_score(), 0);
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_score()), score_t>));
        EXPECT_TRUE((std::is_same_v<decltype(std::move(tmp).get_score()), score_t>));
    }

}

TYPED_TEST(align_result_test, end_coordinate)
{
    using coord_t = decltype(std::declval<TypeParam>().end_coordinate);
    aligned_seq_type seq{'A'_dna4, 'T'_dna4, gap{}, 'C'_dna4, gap{}, gap{}, 'A'_dna4};

    {
        align_result<TypeParam> tmp{TypeParam{1u, 0, {10ul, 10ul}, {0ul, 0ul}, {seq, seq}}};
        EXPECT_EQ(tmp.get_end_coordinate(), (coord_t{10ul, 10ul}));
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_end_coordinate()), coord_t const &>));
        EXPECT_TRUE((std::is_same_v<decltype(std::move(tmp).get_end_coordinate()), coord_t const &>));
    }

    {
        align_result<TypeParam> const tmp{TypeParam{1u, 0, {10ul, 10ul}, {0ul, 0ul}, {seq, seq}}};
        EXPECT_EQ(tmp.get_end_coordinate(), (coord_t{10ul, 10ul}));
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_end_coordinate()), coord_t const &>));
        EXPECT_TRUE((std::is_same_v<decltype(std::move(tmp).get_end_coordinate()), coord_t const &>));
    }
}

TYPED_TEST(align_result_test, begin_coordinate)
{
    using coord_t = decltype(std::declval<TypeParam>().begin_coordinate);
    aligned_seq_type seq{'A'_dna4, 'T'_dna4, gap{}, 'C'_dna4, gap{}, gap{}, 'A'_dna4};

    {
        align_result<TypeParam> tmp{TypeParam{1u, 0, {10ul, 10ul}, {0ul, 0ul}, {seq, seq}}};
        EXPECT_EQ(tmp.get_begin_coordinate(), (coord_t{0ul, 0ul}));
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_begin_coordinate()), coord_t const &>));
        EXPECT_TRUE((std::is_same_v<decltype(std::move(tmp).get_begin_coordinate()), coord_t const &>));
    }

    {
        align_result<TypeParam> const tmp{TypeParam{1u, 0, {10ul, 10ul}, {0ul, 0ul}, {seq, seq}}};
        EXPECT_EQ(tmp.get_begin_coordinate(), (coord_t{0ul, 0ul}));
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_begin_coordinate()), coord_t const &>));
        EXPECT_TRUE((std::is_same_v<decltype(std::move(tmp).get_begin_coordinate()), coord_t const &>));
    }
}

TYPED_TEST(align_result_test, alignment)
{
    using alignment_t = decltype(std::declval<TypeParam>().alignment);
    aligned_seq_type seq{'A'_dna4, 'T'_dna4, gap{}, 'C'_dna4, gap{}, gap{}, 'A'_dna4};

    {
        align_result<TypeParam> tmp{TypeParam{1u, 0, {10ul, 10ul}, {0ul, 0ul}, {seq, seq}}};
        EXPECT_EQ(tmp.get_alignment(), (alignment_t{seq, seq}));
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_alignment()), alignment_t const &>));
        EXPECT_TRUE((std::is_same_v<decltype(std::move(tmp).get_alignment()), alignment_t const &>));
    }

    {
        align_result<TypeParam> const tmp{TypeParam{1u, 0, {10ul, 10ul}, {0ul, 0ul}, {seq, seq}}};
        EXPECT_EQ(tmp.get_alignment(), (alignment_t{seq, seq}));
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_alignment()), alignment_t const &>));
        EXPECT_TRUE((std::is_same_v<decltype(std::move(tmp).get_alignment()), alignment_t const &>));
    }

    if constexpr (tuple_like_concept<alignment_t>)
    {
        align_result<TypeParam> tmp{TypeParam{1u, 0, {10ul, 10ul}, {0ul, 0ul}, {seq, seq}}};
        EXPECT_EQ(std::string{std::get<0>(tmp.get_alignment()) | view::persist | view::to_char},
                  std::string{"AT-C--A"});
        EXPECT_EQ(std::string{std::get<1>(tmp.get_alignment()) | view::persist | view::to_char},
                  std::string{"AT-C--A"});
    }
    else
    {
        align_result<TypeParam> tmp{TypeParam{1u, 0, {10ul, 10ul}, {0ul, 0ul}, {seq, seq}}};
        EXPECT_EQ(std::string{(tmp.get_alignment()[0]) | view::persist | view::to_char}, std::string{"AT-C--A"});
        EXPECT_EQ(std::string{(tmp.get_alignment()[1]) | view::persist | view::to_char}, std::string{"AT-C--A"});
    }
}

TEST(align_result_test, reduced_type)
{
    {
        detail::align_result_value_type tr{2u, 5};
        align_result tmp(tr);
        EXPECT_EQ(tmp.get_id(), 2u);
        EXPECT_EQ(tmp.get_score(), 5);
    }

    {
        detail::align_result_value_type tr{2, 5.0f, std::pair<int, int>{1, -1}};
        align_result tmp(tr);
        EXPECT_EQ(tmp.get_id(), 2);
        EXPECT_FLOAT_EQ(tmp.get_score(), 5.0f);
        EXPECT_EQ((tmp.get_end_coordinate()), (std::pair<int, int>{1, -1}));
    }

    {
        detail::align_result_value_type tr{2, 5.0f, std::pair<int, int>{1, -1}, std::pair<int, int>{10, -10}};
        align_result tmp(tr);
        EXPECT_EQ(tmp.get_id(), 2);
        EXPECT_FLOAT_EQ(tmp.get_score(), 5.0f);
        EXPECT_EQ((tmp.get_end_coordinate()), (std::pair<int, int>{1, -1}));
        EXPECT_EQ((tmp.get_begin_coordinate()), (std::pair<int, int>{10, -10}));
    }
}

TEST(align_result_test, type_deduction)
{
    {
        using coord_t = std::pair<int, int>;
        std::vector<gapped<rna5>> seq{'A'_rna5, 'U'_rna5, gap{}, 'C'_rna5, gap{}, gap{}, 'A'_rna5};

        detail::align_result_value_type tr{2, 5.0, coord_t{1, -1}, coord_t{10,-10}, seq};
        align_result tmp(tr);
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_id()), int>));
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_score()), double>));
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_end_coordinate()), coord_t const &>));
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_begin_coordinate()), coord_t const &>));
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_alignment()), std::vector<gapped<rna5>> const &>));

        EXPECT_EQ(tmp.get_id(), 2);
        EXPECT_DOUBLE_EQ(tmp.get_score(), 5.0);
        EXPECT_EQ((tmp.get_end_coordinate()), (coord_t{1, -1}));
        EXPECT_EQ((tmp.get_begin_coordinate()), (coord_t{10, -10}));
        EXPECT_EQ(tmp.get_alignment(), seq);
    }

    {
        using coord_t = std::pair<int, int>;

        detail::align_result_value_type tr{2, 5.0, coord_t{1, -1}};
        align_result tmp(tr);
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_id()), int>));
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_score()), double>));
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_end_coordinate()), coord_t const &>));
    }

    {
        detail::align_result_value_type tr{2, 5.0};
        align_result tmp(tr);
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_id()), int>));
        EXPECT_TRUE((std::is_same_v<decltype(tmp.get_score()), double>));
    }
}

TEST(align_result_test, empty_type)
{
    detail::align_result_value_type tr{};
    align_result tmp(tr);
    // members must not be accessed
}
