// SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

// generated from test/snippet/alphabet/nucleotide/@target_alphabet@_char_literal.cpp.in

//![main]
#include <seqan3/alphabet/nucleotide/dna16sam.hpp>

int main()
{
    using namespace seqan3::literals;

    seqan3::dna16sam letter1{'A'_dna16sam};
    auto letter2 = 'A'_dna16sam;
}
//![main]
