// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <type_traits>

#include <seqan3/core/add_enum_bitwise_operators.hpp>

namespace seqan3
{
enum class my_enum
{
    ZERO = 0,
    VAL1 = 1,
    VAL2 = 2,
    COMB = 3
};
} // namespace seqan3

template <>
constexpr bool seqan3::add_enum_bitwise_operators<seqan3::my_enum> = true;

TEST(add_enum_bitwise_operators, AND)
{
    seqan3::my_enum e = seqan3::my_enum::VAL1;
    seqan3::my_enum e2 = e & seqan3::my_enum::VAL2;
    EXPECT_EQ(e2, seqan3::my_enum::ZERO);
}

TEST(add_enum_bitwise_operators, OR)
{
    seqan3::my_enum e = seqan3::my_enum::VAL1;
    seqan3::my_enum e2 = e | seqan3::my_enum::VAL2;
    EXPECT_EQ(e2, seqan3::my_enum::COMB);
}

TEST(add_enum_bitwise_operators, XOR)
{
    seqan3::my_enum e = seqan3::my_enum::VAL1;
    seqan3::my_enum e2 = e ^ seqan3::my_enum::VAL2;
    EXPECT_EQ(e2, seqan3::my_enum::COMB);
}

TEST(add_enum_bitwise_operators, NOT)
{
    seqan3::my_enum e = seqan3::my_enum::VAL1;
    seqan3::my_enum e2 = ~e;
    EXPECT_NE(e, e2);
    e2 = ~e2;
    EXPECT_EQ(e, e2);
}

TEST(add_enum_bitwise_operators, AND_ASSIGN)
{
    seqan3::my_enum e = seqan3::my_enum::VAL1;
    e &= seqan3::my_enum::VAL2;
    EXPECT_EQ(e, seqan3::my_enum::ZERO);
}

TEST(add_enum_bitwise_operators, OR_ASSIGN)
{
    seqan3::my_enum e = seqan3::my_enum::VAL1;
    e |= seqan3::my_enum::VAL2;
    EXPECT_EQ(e, seqan3::my_enum::COMB);
}

TEST(add_enum_bitwise_operators, XOR_ASSIGN)
{
    seqan3::my_enum e = seqan3::my_enum::VAL1;
    e ^= seqan3::my_enum::VAL2;
    EXPECT_EQ(e, seqan3::my_enum::COMB);
}
