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
 * \author Marcel Ehrhardt <marcel.ehrhardt AT fu-berlin.de>
 * \brief Contains the declaration of seqan3::detail::alignment_trace_matrix.
 */

#pragma once

#include <seqan3/alignment/matrix/alignment_score_matrix.hpp>
#include <seqan3/alignment/matrix/matrix_concept.hpp>
#include <seqan3/alignment/matrix/trace_directions.hpp>

namespace seqan3::detail
{

//!\brief The declaration of alignment_trace_matrix. Each definition of this
//!       declaration must satisfy seqan3::detail::matrix_concept.
//!\ingroup alignment_matrix
//!\implements seqan3::detail::matrix_concept
template <typename ...>
class alignment_trace_matrix;

/*!\brief A trace matrix represented in a one-dimensional std::vector
 * \ingroup alignment_matrix
 *
 * \details
 *
 * This data structure stores the matrix in a flat way using the
 * std::vector<#entry_type> data structure where each row is stored
 * continuously.
 *
 * ## Example
 *
 * \snippet test/snippet/alignment/matrix/alignment_trace_matrix_vector.cpp code
 *
 * ### Output
 * \include test/snippet/alignment/matrix/alignment_trace_matrix_vector.out
 */
template <>
class alignment_trace_matrix<std::vector<trace_directions>>
    : public row_wise_matrix<trace_directions>
{
public:
    using row_wise_matrix<trace_directions>::row_wise_matrix;
};

/*!\brief A trace matrix that uses an underlying seqan3::detail::alignment_score_matrix
 * \ingroup alignment_matrix
 * \tparam database_type            The type of the database sequence.
 * \tparam query_type               The type of the query sequence.
 * \tparam align_config_type        The type of the alignment config.
 * \tparam ...score_matrix_params_t The template parameters of seqan3::detail::alignment_score_matrix
 *
 * \details
 *
 * This data structure uses directly the score matrix to infer the trace matrix
 * and works for any seqan3::detail::alignment_score_matrix.
 *
 * \todo TODO: Is currently only able to handle the edit distance.
 *
 * ## Example
 *
 * \snippet test/snippet/alignment/matrix/alignment_trace_matrix.cpp code
 *
 * ### Output
 * \include test/snippet/alignment/matrix/alignment_trace_matrix.out
 */
template <typename database_type, typename query_type, typename align_config_type, typename ...score_matrix_params_t>
//!\cond
    requires matrix_concept<alignment_score_matrix<score_matrix_params_t...>> &&
             std::Integral<typename alignment_score_matrix<score_matrix_params_t...>::entry_type>
//!\endcond
class alignment_trace_matrix<database_type, query_type, align_config_type, alignment_score_matrix<score_matrix_params_t...>>
    : public alignment_score_matrix<score_matrix_params_t...>
{
public:
    //!\brief The type of the score matrix.
    using score_matrix_type = alignment_score_matrix<score_matrix_params_t...>;

    //!\brief The type of an entry in the score matrix.
    using score_type = typename score_matrix_type::entry_type;

    //!\copydoc seqan3::detail::matrix_concept::entry_type
    using entry_type = trace_directions;

    /*!\name Constructors, destructor and assignment
     * The copy-constructor, move-constructor, copy-assignment, move-assignment,
     * and destructor are implicitly defined.
     * \{
     */
     alignment_trace_matrix() = default;
     alignment_trace_matrix(alignment_trace_matrix const &) = default;
     alignment_trace_matrix(alignment_trace_matrix &&) = default;
     alignment_trace_matrix & operator=(alignment_trace_matrix const &) = default;
     alignment_trace_matrix & operator=(alignment_trace_matrix &&) = default;

    /*!\brief Construct the trace matrix by using a score_matrix.
     * \param database     The database sequence.
     * \param query        The query sequence.
     * \param config       The alignment config.
     * \param score_matrix The score matrix.
     */
    alignment_trace_matrix(database_type database, query_type query, align_config_type config, score_matrix_type score_matrix)
        : score_matrix_type(std::move(score_matrix)),
          _database{std::forward<database_type>(database)},
          _query{std::forward<query_type>(query)},
          _config{std::forward<align_config_type>(config)}
    {}
    //!\}

    //!\copydoc seqan3::detail::matrix_concept::rows
    using score_matrix_type::rows;
    //!\copydoc seqan3::detail::matrix_concept::cols
    using score_matrix_type::cols;

    //!\brief The trace directions of the matrix at position (*row*, *col*).
    entry_type at(size_t const row, size_t const col) const noexcept
    {
        entry_type direction{};

        if (is_trace_diagonal(row, col))
            direction |= entry_type::diagonal;

        if (is_trace_up(row, col))
            direction |= entry_type::up;

        if (is_trace_left(row, col))
            direction |= entry_type::left;

        return direction;
    }

    //!\brief Access to the score_matrix.
    score_matrix_type const & score_matrix() const noexcept
    {
        return *this;
    }

private:

    //!\brief Does the trace come from the above entry?
    bool is_trace_up(size_t const row, size_t const col) const noexcept
    {
        // TODO: use the alignment_config to calculate the score
        score_type gap = 1;

        score_type curr = score_matrix().at(row, col);
        score_type up = row == 0 ? col : score_matrix().at(row - 1, col);
        return curr == up + gap;
    }

    //!\brief Does the trace come from the left entry?
    bool is_trace_left(size_t const row, size_t const col) const noexcept
    {
        // TODO: use the alignment_config to calculate the score
        score_type gap = 1;

        score_type curr = score_matrix().at(row, col);
        score_type left = col == 0 ? row : score_matrix().at(row, col - 1);
        return curr == left + gap;
    }

    //!\brief Does the trace come from the diagonal entry?
    bool is_trace_diagonal(size_t const row, size_t const col) const noexcept
    {
        // TODO: use the alignment_config to calculate the score
        score_type match = 0;
        score_type mismatch = 1;

        score_type curr = score_matrix().at(row, col);
        if (col == 0 || row == 0)
            return false;

        score_type diag = score_matrix().at(row - 1, col - 1);
        bool is_match = _query[row - 1] == _database[col - 1];

        return (is_match && curr == diag + match) ||
              (!is_match && curr == diag + mismatch);
    }

    //!\brief The database sequence.
    database_type _database;
    //!\brief The query sequence.
    query_type _query;
    //!\brief The alignment config.
    align_config_type _config;
};

/*!\name Type deduction guides
 * \relates seqan3::detail::alignment_trace_matrix
 * \{
 */
alignment_trace_matrix(std::vector<trace_directions>, size_t rows, size_t cols)
    -> alignment_trace_matrix<std::vector<trace_directions>>;

template <typename database_t, typename query_t, typename align_config_t, typename alignment_t, typename ...options_t>
alignment_trace_matrix(database_t && database, query_t && query, align_config_t && config, alignment_score_matrix<alignment_t, options_t...>)
    -> alignment_trace_matrix<database_t, query_t, align_config_t, alignment_score_matrix<alignment_t, options_t...>>;
//!\}

} // namespace seqan3::detail
