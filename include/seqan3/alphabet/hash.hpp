// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \author Enrico Seiler <enrico.seiler AT fu-berlin.de>
 * \brief Provides overloads for std::hash.
 */

#pragma once

#include <seqan3/alphabet/adaptation/all.hpp> // https://github.com/seqan/seqan3/issues/3015

namespace std
{
/*!\brief Struct for hashing a character.
 * \ingroup alphabet
 * \tparam alphabet_t The type of character to hash; must model seqan3::semialphabet.
 * \details
 * \stableapi{Since version 3.1.}
 */
template <typename alphabet_t>
    requires seqan3::semialphabet<alphabet_t>
struct hash<alphabet_t>
{
    /*!\brief Compute the hash for a character.
     * \param[in] character The character to process; must model seqan3::semialphabet.
     *
     * \returns The rank of the character.
     * \sa seqan3::to_rank.
     * \details
     * \stableapi{Since version 3.1.}
     */
    size_t operator()(alphabet_t const character) const noexcept
    {
        return seqan3::to_rank(character);
    }
};

} // namespace std
