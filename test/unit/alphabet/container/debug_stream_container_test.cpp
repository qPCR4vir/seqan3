// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <seqan3/alphabet/container/bitpacked_sequence.hpp>
#include <seqan3/alphabet/detail/debug_stream_alphabet.hpp>
#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/core/debug_stream/range.hpp>
#include <seqan3/utility/container/small_vector.hpp>

using seqan3::operator""_dna4;

template <typename T>
class debug_stream_test : public ::testing::Test
{};

using container_types = ::testing::Types<std::vector<seqan3::dna4>,
                                         seqan3::bitpacked_sequence<seqan3::dna4>,
                                         seqan3::small_vector<seqan3::dna4, 1000>>;

TYPED_TEST_SUITE(debug_stream_test, container_types, );

TYPED_TEST(debug_stream_test, container)
{
    TypeParam t1{'A'_dna4, 'C'_dna4, 'C'_dna4, 'G'_dna4, 'T'_dna4};

    std::ostringstream o;
    seqan3::debug_stream_type my_stream{o};

    my_stream << TypeParam{};

    o.flush();
    EXPECT_EQ(o.str(), "");

    my_stream << t1;

    o.flush();
    EXPECT_EQ(o.str(), "ACCGT");
}
