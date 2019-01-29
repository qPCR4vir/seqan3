// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \author Marcel Ehrhardt <marcel.ehrhardt AT fu-berlin.de>
 * \author David Heller <david.heller AT fu-berlin.de>
 * \brief Contains seqan3::gapped.
 */

#pragma once

#include <seqan3/alphabet/gap/gap.hpp>
#include <seqan3/alphabet/composition/union_composition.hpp>

namespace seqan3
{


/*!\brief Extends a given alphabet with a gap character.
 * \ingroup gap
 * \tparam alphabet_t Type of the letter, e.g. dna4; must satisfy seqan3::alphabet_concept.
 *
 * The gapped alphabet represents the union of a given alphabet and the
 * seqan3::gap alphabet (e.g. the four letter DNA alphabet + a gap character).
 *
 * The gapped alphabet may be brace initialized from the static letter members of the underlying alphabet and the
 * seqan3::gap alphabet. Note that you cannot assign the alphabet by using letters of type `char`, but you instead have
 * to use the function assign_char() of the underlying alphabet or seqan3::gap::assign_char().
 *
 * \snippet test/snippet/alphabet/gap/gapped.cpp general
 *
 * \sa For more details see union_composition, which is the base class and more general than the gapped alphabet.
 */
template <typename alphabet_t>
//!\cond
    requires alphabet_concept<alphabet_t>
//!\endcond
using gapped = union_composition<alphabet_t, gap>;

} // namespace seqan3
