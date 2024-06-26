// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \author Marcel Ehrhardt <marcel.ehrhardt AT fu-berlin.de>
 * \author David Heller <david.heller AT fu-berlin.de>
 * \brief Provides seqan3::gap.
 */

#pragma once

#include <cassert>

#include <seqan3/alphabet/alphabet_base.hpp>

namespace seqan3
{

/*!\brief The alphabet of a gap character '-'
 * \ingroup alphabet_gap
 * \implements seqan3::writable_alphabet
 * \if DEV \implements seqan3::detail::writable_constexpr_alphabet \endif
 * \implements seqan3::trivially_copyable
 * \implements seqan3::standard_layout
 * \implements std::regular
 *
 * The alphabet always has the same value ('-').
 *
 * \include test/snippet/alphabet/gap/gap.cpp
 *
 * \stableapi{Since version 3.1.}
 */

class gap : public alphabet_base<gap, 1, char>
{
private:
    //!\brief The base class.
    using base_t = alphabet_base<gap, 1, char>;

    //!\brief Befriend seqan3::alphabet_base.
    friend base_t;

    //!\copydoc seqan3::dna4::rank_to_char
    static constexpr char_type rank_to_char(rank_type const)
    {
        return '-';
    }

    //!\copydoc seqan3::dna4::char_to_rank
    static constexpr rank_type char_to_rank(char_type const)
    {
        return 0;
    }

public:
    /*!\name Constructors, destructor and assignment
     * \{
     */
    constexpr gap() noexcept : base_t{}
    {}                                                //!< Defaulted.
    constexpr gap(gap const &) = default;             //!< Defaulted.
    constexpr gap(gap &&) = default;                  //!< Defaulted.
    constexpr gap & operator=(gap const &) = default; //!< Defaulted.
    constexpr gap & operator=(gap &&) = default;      //!< Defaulted.
    ~gap() = default;                                 //!< Defaulted.

    using base_t::base_t;
    //!\}
};

} // namespace seqan3
