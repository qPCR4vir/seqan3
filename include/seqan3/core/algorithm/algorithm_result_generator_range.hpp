// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2022, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2022, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides seqan3::detail::algorithm_result_generator_range.
 * \author Rene Rahn <rene.rahn AT fu-berlin.de>
 */

#pragma once

#include <concepts>
#include <memory>
#include <ranges>

#include <seqan3/core/platform.hpp>

namespace seqan3
{

/*!\brief An input range over the algorithm results generated by the underlying algorithm executor.
 * \ingroup core_algorithm
 * \implements std::ranges::input_range
 *
 * \tparam algorithm_executor_type The type of the underlying algorithm executor; must be of type
 *                                 seqan3::detail::algorithm_executor_blocking.
 *
 * \details
 *
 * Provides a lazy input-range interface over the algorithm results generated by the underlying algorithm
 * executor. The algorithms are computed in a lazy fashion, such that when incrementing the next algorithm
 * result is fetched from the executor. The algorithm result will be cached such that dereferencing the
 * associated iterator is constant.
 *
 * \if DEV
 * Note that the required type is not enforced in order to test this class without adding the entire machinery for
 * the seqan3::detail::algorithm_executor_blocking.
 * \endif
 *
 * \see core_algorithm
 */
template <typename algorithm_executor_type>
class algorithm_result_generator_range
{
    static_assert(!std::is_const_v<algorithm_executor_type>, "Cannot create an algorithm stream over a const buffer.");

    //!\brief The optional type returned by the seqan3::detail::algorithm_executor_blocking
    using optional_type = decltype(std::declval<algorithm_executor_type>().next_result());
    //!\brief The actual algorithm result type.
    using algorithm_result_type = typename optional_type::value_type;

    class algorithm_range_iterator;

    //!\brief Befriend the iterator type.
    friend class algorithm_range_iterator;

public:
    /*!\name Constructors, destructor and assignment
     * \{
     */
    algorithm_result_generator_range() = default;                                        //!< Defaulted.
    algorithm_result_generator_range(algorithm_result_generator_range const &) = delete; //!< This is a move-only type.
    algorithm_result_generator_range(algorithm_result_generator_range &&) = default;     //!< Defaulted.
    algorithm_result_generator_range &
    operator=(algorithm_result_generator_range const &) = delete; //!< This is a move-only type.
    algorithm_result_generator_range & operator=(algorithm_result_generator_range &&) = default; //!< Defaulted.
    ~algorithm_result_generator_range() = default;                                               //!< Defaulted.

    //!\brief Explicit deletion to forbid copy construction of the underlying executor.
    explicit algorithm_result_generator_range(algorithm_executor_type const & algorithm_executor) = delete;

    /*!\brief Constructs a new algorithm range by taking ownership over the passed algorithm buffer.
     *
     * \param[in] algorithm_executor   The buffer to take ownership from.
     *
     * \details
     *
     * Constructs a new algorithm range by taking ownership over the passed algorithm buffer.
     */
    explicit algorithm_result_generator_range(algorithm_executor_type && algorithm_executor) :
        algorithm_executor_ptr{std::make_unique<algorithm_executor_type>(std::move(algorithm_executor))}
    {}
    //!\}

    /*!\name Iterators
     * \{
     */

    /*!\brief Returns an iterator to the first element of the algorithm range.
     * \return An iterator to the first element.
     *
     * \details
     *
     * Invocation of this function will trigger the computation of the first algorithm.
     */
    constexpr algorithm_range_iterator begin()
    {
        return algorithm_range_iterator{*this};
    }

    //!\brief This range is not const-iterable.
    algorithm_range_iterator begin() const = delete;

    /*!\brief Returns a sentinel signaling the end of the algorithm range.
     * \return a sentinel.
     *
     * \details
     *
     * The algorithm range is an input range and the end is reached when the internal buffer over the algorithm
     * results has signaled end-of-stream.
     */
    constexpr std::default_sentinel_t end() noexcept
    {
        return std::default_sentinel;
    }

    //!\brief This range is not const-iterable.
    constexpr std::default_sentinel_t end() const = delete;
    //!\}

protected:
    /*!\brief Receives the next algorithm result from the executor buffer.
     *
     * \returns `true` if a new algorithm result could be fetched, otherwise `false`.
     *
     * \details
     *
     * Fetches the next element from the underlying algorithm executor if available.
     */
    bool next()
    {
        if (!algorithm_executor_ptr)
            throw std::runtime_error{"No algorithm execution buffer available."};

        if (auto opt = algorithm_executor_ptr->next_result(); opt.has_value())
        {
            cache = std::move(*opt);
            return true;
        }

        return false;
    }

private:
    //!\brief The underlying executor buffer.
    std::unique_ptr<algorithm_executor_type> algorithm_executor_ptr{};
    //!\brief Stores last read element.
    algorithm_result_type cache{};
};

/*!\name Type deduction guide
 * \relates seqan3::algorithm_result_generator_range
 * \{
 */

//!\brief Deduces from the passed algorithm_executor_type
template <typename algorithm_executor_type>
algorithm_result_generator_range(algorithm_executor_type &&)
    -> algorithm_result_generator_range<std::remove_reference_t<algorithm_executor_type>>;
//!\}

/*!\brief The iterator of seqan3::detail::algorithm_result_generator_range.
 * \implements std::input_iterator
 */
template <typename algorithm_executor_type>
class algorithm_result_generator_range<algorithm_executor_type>::algorithm_range_iterator
{
public:
    /*!\name Associated types
     * \{
     */
    //!\brief Type for distances between iterators.
    using difference_type = std::ptrdiff_t;
    //!\brief Value type of container elements.
    using value_type = algorithm_result_type;
    //!\brief Use reference type defined by container.
    using reference = std::add_lvalue_reference_t<value_type>;
    //!\brief Pointer type is pointer of container element type.
    using pointer = std::add_pointer_t<value_type>;
    //!\brief Sets iterator category as input iterator.
    using iterator_category = std::input_iterator_tag;
    //!\}

    /*!\name Constructors, destructor and assignment
     * \{
     */
    constexpr algorithm_range_iterator() noexcept = default;                                             //!< Defaulted.
    constexpr algorithm_range_iterator(algorithm_range_iterator const &) noexcept = default;             //!< Defaulted.
    constexpr algorithm_range_iterator(algorithm_range_iterator &&) noexcept = default;                  //!< Defaulted.
    constexpr algorithm_range_iterator & operator=(algorithm_range_iterator const &) noexcept = default; //!< Defaulted.
    constexpr algorithm_range_iterator & operator=(algorithm_range_iterator &&) noexcept = default;      //!< Defaulted.
    ~algorithm_range_iterator() = default;                                                               //!< Defaulted.

    //!\brief Construct from alignment stream.
    explicit constexpr algorithm_range_iterator(algorithm_result_generator_range & range) :
        range_ptr(std::addressof(range))
    {
        ++(*this); // Fetch the next element.
    }
    //!\}

    /*!\name Access operators
     * \{
     */

    /*!\brief Access the pointed-to element.
     * \returns A reference to the current element.
     */
    //!\brief Returns the current algorithm result.
    reference operator*() const noexcept
    {
        return range_ptr->cache;
    }

    //!\brief Returns a pointer to the current algorithm result.
    pointer operator->() const noexcept
    {
        return &range_ptr->cache;
    }
    //!\}

    /*!\name Increment operators
     * \{
     */
    //!\brief Increments the iterator by one.
    algorithm_range_iterator & operator++(/*pre*/)
    {
        assert(range_ptr != nullptr);

        at_end = !range_ptr->next();
        return *this;
    }

    //!\brief Returns an iterator incremented by one.
    void operator++(int /*post*/)
    {
        ++(*this);
    }
    //!\}

    /*!\name Comparison operators
     * \{
     */
    //!\brief Checks whether lhs is equal to the sentinel.
    friend constexpr bool operator==(algorithm_range_iterator const & lhs, std::default_sentinel_t const &) noexcept
    {
        return lhs.at_end;
    }

    //!\brief Checks whether `lhs` is equal to `rhs`.
    friend constexpr bool operator==(std::default_sentinel_t const & lhs, algorithm_range_iterator const & rhs) noexcept
    {
        return rhs == lhs;
    }

    //!\brief Checks whether `*this` is not equal to the sentinel.
    friend constexpr bool operator!=(algorithm_range_iterator const & lhs, std::default_sentinel_t const & rhs) noexcept
    {
        return !(lhs == rhs);
    }

    //!\brief Checks whether `lhs` is not equal to `rhs`.
    friend constexpr bool operator!=(std::default_sentinel_t const & lhs, algorithm_range_iterator const & rhs) noexcept
    {
        return rhs != lhs;
    }
    //!\}

private:
    //!\brief Pointer to the underlying range.
    algorithm_result_generator_range * range_ptr{};
    //!\brief Indicates the end of the underlying resource.
    bool at_end{true};
};

} // namespace seqan3
