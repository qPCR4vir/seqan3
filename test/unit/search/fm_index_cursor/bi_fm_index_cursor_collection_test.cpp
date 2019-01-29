// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

#include "bi_fm_index_cursor_collection_test_template.hpp"

INSTANTIATE_TYPED_TEST_CASE_P(dna4, bi_fm_index_cursor_collection_test,
                              bi_fm_index_cursor<bi_fm_index<std::vector<std::vector<dna4>>>>);
