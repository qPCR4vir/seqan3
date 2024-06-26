// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \author Hannes Hauswedell <hannes.hauswedell AT fu-berlin.de>
 * \brief Provides seqan3::debug_stream and related types.
 */

#pragma once

#include <variant>

#include <seqan3/core/debug_stream/debug_stream_type.hpp>
#include <seqan3/core/detail/template_inspection.hpp>
#include <seqan3/utility/type_traits/basic.hpp>

namespace seqan3
{
/*!\name Formatted output overloads
 * \{
 */
/*!\brief A std::variant can be printed by visiting the stream operator for the corresponding type.
 * \tparam    variant_type The type of the variant.
 * \param[in] s            The seqan3::debug_stream.
 * \param[in] v            The variant.
 * \relates seqan3::debug_stream_type
 *
 * \details
 *
 * Note that in case the variant is valueless(_by_exception), nothing is printed.
 */
template <typename char_t, typename variant_type>
    requires detail::is_type_specialisation_of_v<std::remove_cvref_t<variant_type>, std::variant>
inline debug_stream_type<char_t> & operator<<(debug_stream_type<char_t> & s, variant_type && v)
{
    if (!v.valueless_by_exception())
        std::visit(
            [&s](auto && arg)
            {
                s << arg;
            },
            v);
    else
        s << "<VALUELESS_VARIANT>";
    return s;
}

//!\}

} // namespace seqan3
