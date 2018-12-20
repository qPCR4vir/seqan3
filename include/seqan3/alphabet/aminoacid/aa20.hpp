// ============================================================================
//                 SeqAn - The Library for Sequence Analysis
// ============================================================================
//
// Copyright (c) 2006-2018, Knut Reinert & Freie Universitaet Berlin
// Copyright (c) 2016-2018, Knut Reinert & MPI Molekulare Genetik
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Knut Reinert or the FU Berlin nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL KNUT REINERT OR THE FU BERLIN BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ============================================================================

/*!\file
 * \author Joshua Kim <joshua.kim AT fu-berlin.de>
 * \brief Contains seqan3::aa20, container aliases and string literals.
 */

#pragma once

#include <vector>

#include <seqan3/alphabet/aminoacid/aminoacid_base.hpp>
#include <seqan3/io/stream/char_operations.hpp>

namespace seqan3
{
/*!\brief The canonical amino acid alphabet.
 * \ingroup aminoacid
 * \implements seqan3::aminoacid_concept
 * \implements seqan3::detail::constexpr_alphabet_concept
 * \implements seqan3::trivially_copyable_concept
 * \implements seqan3::standard_layout_concept
 *
 * \details
 * The alphabet consists of letters A, C, D, E, F, G, H, I, K, L, M, N, P, Q, R, S, T, V, W, Y
 *
 * The alphabet may be brace initialized from the static letter members (see above). Note that you cannot assign
 * regular characters, but additional functions for this are available.
 *
 * *Note:* Letters which belong in the extended alphabet will be automatically converted based on the frequency
 * of their options.\n Terminator characters are converted to W, because the most commonly occurring
 * stop codon in higher eukaryotes is UGA<sup>2</sup>.
 * Anything unknown is converted to S, because it occurs most frequently across 53 vertebrates<sup>1</sup>.
 *
 * |Input Letter  |Converts to  |
 * |--------------|-------------|
 * |B             |D<sup>1</sup>|
 * |J             |L<sup>1</sup>|
 * |O             |L<sup>1</sup>|
 * |U             |C<sup>1</sup>|
 * |Z             |E<sup>1</sup>|
 * |X (Unknown)   |S<sup>1</sup>|
 * |* (Terminator)|W<sup>2</sup>|
 * <sup><b>1</b></sup>King, J. L., & Jukes, T. H. (1969). Non-Darwinian Evolution.
 * Science, 164(3881), 788-798. doi:10.1126/science.164.3881.788\n
 * <sup><b>2</b></sup>Trotta, E. (2016). Selective forces and mutational biases drive stop codon usage
 * in the human genome: a comparison with sense codon usage.
 * BMC Genomics, 17, 366. http://doi.org/10.1186/s12864-016-2692-4
 *
 * \snippet test/snippet/alphabet/aminoacid/aa20.cpp construction
 */
class aa20 : public aminoacid_base<aa20, 20>
{
private:
    //!\brief The base class.
    using base_t = aminoacid_base<aa20, 20>;

    //!\brief Befriend seqan3::aminoacid_base.
    friend base_t;
    //!\cond \brief Befriend seqan3::alphabet_base.
    friend base_t::base_t;
    //!\endcond

public:
    /*!\name Constructors, destructor and assignment
     * \{
     */
    constexpr aa20() : base_t{} {}
    constexpr aa20(aa20 const &) = default;
    constexpr aa20(aa20 &&) = default;
    constexpr aa20 & operator=(aa20 const &) = default;
    constexpr aa20 & operator=(aa20 &&) = default;
    ~aa20() = default;

    using base_t::base_t;
    //!\}

protected:
    //!\brief Value to char conversion table.
    static constexpr char_type rank_to_char[value_size]
    {
        'A',
        'C',
        'D',
        'E',
        'F',
        'G',
        'H',
        'I',
        'K',
        'L',
        'M',
        'N',
        'P',
        'Q',
        'R',
        'S',
        'T',
        'V',
        'W',
        'Y',
    };

    //!\brief Char to value conversion table.
    static constexpr std::array<rank_type, 256> char_to_rank
    {
        [] () constexpr
        {
            std::array<rank_type, 256> ret{};

            // initialize with UNKNOWN (std::array::fill unfortunately not constexpr)
            for (auto & c : ret)
                c = 15; // value of 'S', because that appears most frequently

            // reverse mapping for characters and their lowercase
            for (rank_type rnk = 0u; rnk < value_size; ++rnk)
            {
                ret[static_cast<rank_type>(         rank_to_char[rnk]) ] = rnk;
                ret[static_cast<rank_type>(to_lower(rank_to_char[rnk]))] = rnk;
            }

            ret['B'] = ret['D']; ret['b'] = ret['D']; // Convert b (either D/N) to D, since D occurs more frequently.
            ret['J'] = ret['L']; ret['j'] = ret['L']; // Convert j (either I/L) to L, since L occurs more frequently.
            ret['O'] = ret['L']; ret['o'] = ret['L']; // Convert Pyrrolysine to lysine.
            ret['U'] = ret['C']; ret['u'] = ret['C']; // Convert Selenocysteine to cysteine.
            ret['X'] = ret['S']; ret['x'] = ret['S']; // Convert unknown amino acids to serine.
            ret['Z'] = ret['E']; ret['z'] = ret['E']; // Convert z (either E/Q) to E, since E occurs more frequently.
            ret['*'] = ret['W']; // The most common stop codon is UGA. This is most similar to a Tryptophan.
            return ret;
        }()
    };
};

} // namespace seqan3

// ------------------------------------------------------------------
// containers
// ------------------------------------------------------------------

namespace seqan3
{
//!\brief Alias for an std::vector of seqan3::aa20.
//!\relates aa20
using aa20_vector = std::vector<aa20>;

} // namespace seqan3

// ------------------------------------------------------------------
// literals
// ------------------------------------------------------------------

namespace seqan3
{

/*!\brief aa20 literal
 * \relates seqan3::aa20
 * \returns seqan3::aa20_vector
 *
 * You can use this string literal to easily assign to aa20_vector:
 *
 *\snippet test/snippet/alphabet/aminoacid/aa20.cpp literal
 *
 * \attention
 * All seqan3 literals are in the namespace seqan3!
 */

inline aa20_vector operator""_aa20(const char * s, std::size_t n)
{
    aa20_vector r;
    r.resize(n);

    for (size_t i = 0; i < n; ++i)
        r[i].assign_char(s[i]);

    return r;
}

} // namespace seqan3
