// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <seqan3/alignment/pairwise/align_pairwise.hpp>

#include "fixture/local_affine_unbanded.hpp"
#include "pairwise_alignment_single_test_template.hpp"

using pairwise_local_affine_unbanded_testing_types =
    ::testing::Types<pairwise_alignment_fixture<&seqan3::test::alignment::fixture::local::affine::unbanded::dna4_01>,
                     pairwise_alignment_fixture<&seqan3::test::alignment::fixture::local::affine::unbanded::dna4_02>,
                     pairwise_alignment_fixture<&seqan3::test::alignment::fixture::local::affine::unbanded::dna4_03>,
                     pairwise_alignment_fixture<&seqan3::test::alignment::fixture::local::affine::unbanded::dna4_04>,
                     pairwise_alignment_fixture<&seqan3::test::alignment::fixture::local::affine::unbanded::dna4_05>,
                     pairwise_alignment_fixture<&seqan3::test::alignment::fixture::local::affine::unbanded::rna5_01>,
                     pairwise_alignment_fixture<&seqan3::test::alignment::fixture::local::affine::unbanded::aa27_01>,
                     pairwise_alignment_fixture<&seqan3::test::alignment::fixture::local::affine::unbanded::aa27_02>>;

INSTANTIATE_TYPED_TEST_SUITE_P(pairwise_local_affine_unbanded,
                               pairwise_alignment_test,
                               pairwise_local_affine_unbanded_testing_types, );
