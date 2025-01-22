// SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

/*!\file
 * \author Enrico Seiler <enrico.seiler AT fu-berlin.de>
 * \brief Provides seqan3::views::kmer_hash.
 */

#pragma once

#include <seqan3/alphabet/concept.hpp>
#include <seqan3/alphabet/range/hash.hpp>
#include <seqan3/search/kmer_index/shape.hpp>
#include <seqan3/utility/math.hpp>

namespace seqan3::detail
{
// ---------------------------------------------------------------------------------------------------------------------
// kmer_hash_view class
// ---------------------------------------------------------------------------------------------------------------------

/*!\brief The type returned by seqan3::views::kmer_hash.
 * \tparam urng_t The type of the underlying ranges, must model std::forward_range, the reference type must model
 *                seqan3::semialphabet.
 * \implements std::ranges::view
 * \implements std::ranges::random_access_range
 * \implements std::ranges::sized_range
 * \ingroup search_views
 *
 * \details
 *
 * Note that most members of this class are generated by std::ranges::view_interface which is not yet documented here.
 */
template <std::ranges::view urng_t>
class kmer_hash_view : public std::ranges::view_interface<kmer_hash_view<urng_t>>
{
private:
    static_assert(std::ranges::forward_range<urng_t>, "The kmer_hash_view only works on forward_ranges");
    static_assert(semialphabet<std::ranges::range_reference_t<urng_t>>,
                  "The reference type of the underlying range must model seqan3::semialphabet.");

    //!\brief The underlying range.
    urng_t urange;

    //!\brief The shape to use.
    shape shape_;

    template <bool const_range>
    class basic_iterator;

    //!\brief The maximum shape count for the given alphabet.
    static inline int max_shape_count = 64 / std::log2(alphabet_size<std::ranges::range_reference_t<urng_t>>);

    //!\brief Throws the exception for validate_shape().
    [[noreturn]] void shape_too_long_error() const
    {
        std::string message{"The shape is too long for the given alphabet.\n"};
        message += "Alphabet: ";
        // Note: Since we want the alphabet type name, std::ranges::range_value_t is the better choice.
        // For seqan3::bitpacked_sequence<seqan3::dna4>:
        // reference_t: seqan3::bitpacked_sequence<seqan3::dna4>::reference_proxy_type
        // value_t: seqan3::dna4
        message += detail::type_name_as_string<std::remove_cvref_t<std::ranges::range_value_t<urng_t>>>;
        message += "\nMaximum shape count: ";
        message += std::to_string(max_shape_count);
        message += "\nGiven shape count: ";
        message += std::to_string(shape_.count());
        throw std::invalid_argument{message};
    }

    //!\brief Checks that the shape is not too long for the given alphabet.
    inline void validate_shape() const
    {
        if (shape_.count() > max_shape_count)
            shape_too_long_error();
    }

public:
    /*!\name Constructors, destructor and assignment
     * \{
     */
    kmer_hash_view()
        requires std::default_initializable<urng_t>
    = default;                                                        //!< Defaulted.
    kmer_hash_view(kmer_hash_view const & rhs) = default;             //!< Defaulted.
    kmer_hash_view(kmer_hash_view && rhs) = default;                  //!< Defaulted.
    kmer_hash_view & operator=(kmer_hash_view const & rhs) = default; //!< Defaulted.
    kmer_hash_view & operator=(kmer_hash_view && rhs) = default;      //!< Defaulted.
    ~kmer_hash_view() = default;                                      //!< Defaulted.

    /*!\brief Construct from a view and a given shape.
     * \throws std::invalid_argument if hashes resulting from the shape/alphabet combination cannot be represented in
     *         `uint64_t`, i.e. \f$s>\frac{64}{\log_2\sigma}\f$ with shape size \f$s\f$ and alphabet size \f$\sigma\f$.
     */
    explicit kmer_hash_view(urng_t urange_, shape const & s_) : urange{std::move(urange_)}, shape_{s_}
    {
        validate_shape();
    }

    /*!\brief Construct from a non-view that can be view-wrapped and a given shape.
     * \throws std::invalid_argument if hashes resulting from the shape/alphabet combination cannot be represented in
     *         `uint64_t`, i.e. \f$s>\frac{64}{\log_2\sigma}\f$ with shape size \f$s\f$ and alphabet size \f$\sigma\f$.
     */
    template <typename rng_t>
        requires (!std::same_as<std::remove_cvref_t<rng_t>, kmer_hash_view>) && std::ranges::viewable_range<rng_t>
                  && std::constructible_from<urng_t, std::ranges::ref_view<std::remove_reference_t<rng_t>>>
    explicit kmer_hash_view(rng_t && urange_, shape const & s_) :
        urange{std::views::all(std::forward<rng_t>(urange_))},
        shape_{s_}
    {
        validate_shape();
    }
    //!\}

    /*!\name Iterators
     * \{
     */
    /*!\brief Returns an iterator to the first element of the range.
     * \returns Iterator to the first element.
     *
     * \details
     *
     * ### Complexity
     *
     * Constant.
     *
     * ### Exceptions
     *
     * No-throw guarantee.
     */
    auto begin() noexcept
    {
        return basic_iterator<false>{std::ranges::begin(urange), std::ranges::end(urange), shape_};
    }

    //!\copydoc begin()
    auto begin() const noexcept
        requires const_iterable_range<urng_t>
    {
        return basic_iterator<true>{std::ranges::begin(urange), std::ranges::end(urange), shape_};
    }

    /*!\brief Returns an iterator to the element following the last element of the range.
     * \returns Iterator to the end.
     *
     * \details
     *
     * This element acts as a placeholder; attempting to dereference it results in undefined behaviour.
     *
     * ### Complexity
     *
     * Constant.
     *
     * ### Exceptions
     *
     * No-throw guarantee.
     */
    auto end() noexcept
    {
        // Assigning the end iterator to the text_right iterator of the basic_iterator only works for common ranges.
        if constexpr (std::ranges::common_range<urng_t>)
            return basic_iterator<false>{std::ranges::begin(urange), std::ranges::end(urange), shape_, true};
        else
            return std::ranges::end(urange);
    }

    //!\copydoc end()
    auto end() const noexcept
        requires const_iterable_range<urng_t>
    {
        // Assigning the end iterator to the text_right iterator of the basic_iterator only works for common ranges.
        if constexpr (std::ranges::common_range<urng_t const>)
            return basic_iterator<true>{std::ranges::begin(urange), std::ranges::end(urange), shape_, true};
        else
            return std::ranges::cend(urange);
    }
    //!\}

    /*!\brief Returns the size of the range, if the underlying range is a std::ranges::sized_range.
     * \returns Size of range.
     */
    auto size()
        requires std::ranges::sized_range<urng_t>
    {
        using size_type = std::ranges::range_size_t<urng_t>;
        return std::max<size_type>(std::ranges::size(urange) + 1, shape_.size()) - shape_.size();
    }

    //!\copydoc size()
    auto size() const
        requires std::ranges::sized_range<urng_t const>
    {
        using size_type = std::ranges::range_size_t<urng_t const>;
        return std::max<size_type>(std::ranges::size(urange) + 1, shape_.size()) - shape_.size();
    }
};

/*!\brief Iterator for calculating hash values via a given seqan3::shape.
 * \tparam urng_t Type of the text. Must model std::forward_range. Reference type must model seqan3::semialphabet.
 *
 * \details
 *
 * The basic_iterator can be used to iterate over the hash values of a text. The basic_iterator needs an iterator of
 * the text and a seqan3::shape that defines how to hash the text.
 *
 * Depending on the type of the iterator passed to the basic_iterator, different functionality is available:
 *
 * | Concept modelled by passed text iterator | Available functions             |
 * |------------------------------------------|---------------------------------|
 * | std::forward_iterator                    | \ref basic_iterator_comparison_kmer_hash "Comparison operators"<br>\ref operator++ "Pre-increment (++it)"<br>\ref operator++(int) "Post-increment (it++)"<br>\ref operator* "Indirection operator (*it)" |
 * | std::bidirectional_iterator              | \ref operator-- "Pre-decrement (--it)"<br>\ref operator--(int) "Post-decrement (it--)" |
 * | std::random_access_iterator              | \ref operator+= "Forward (it +=)"<br>\ref operator+ "Forward copy (it +)"<br>\ref operator-= "Decrement(it -=)"<br>\ref basic_iterator_operator-decrement "Decrement copy (it -)"<br>\ref basic_iterator_operator-difference "Difference (it1 - it2)"<br>\ref operator[] "Subscript (it[])" |
 *
 * When using a gapped seqan3::shape, the `0`s of the seqan3::shape are virtually removed from the hashed k-mer.
 * Note that any shape is expected to start with a `1` and end with a `1`.
 *
 * \experimentalapi
 *
 * ### Implementation detail
 *
 * To avoid dereferencing the sentinel when iterating, the basic_iterator computes the hash value up until
 * the second to last position and performs the addition of the last position upon
 * access (\ref operator* and \ref operator[]).
 */
template <std::ranges::view urng_t>
template <bool const_range>
class kmer_hash_view<urng_t>::basic_iterator :
    public maybe_iterator_category<maybe_const_iterator_t<const_range, urng_t>>
{
private:
    //!\brief The iterator type of the underlying range.
    using it_t = maybe_const_iterator_t<const_range, urng_t>;
    //!\brief The sentinel type of the underlying range.
    using sentinel_t = maybe_const_sentinel_t<const_range, urng_t>;

    template <bool other_const_range>
    friend class basic_iterator;

public:
    /*!\name Associated types
     * \{
     */
    //!\brief Type for distances between iterators.
    using difference_type = typename std::iter_difference_t<it_t>;
    //!\brief Value type of this iterator.
    using value_type = size_t;
    //!\brief The pointer type.
    using pointer = void;
    //!\brief Reference to `value_type`.
    using reference = value_type;
    //!\brief Tag this class depending on which concept `it_t` models.
    using iterator_concept = std::conditional_t<std::contiguous_iterator<it_t>,
                                                typename std::random_access_iterator_tag,
                                                detail::iterator_concept_tag_t<it_t>>;
    //!\}

    /*!\name Constructors, destructor and assignment
     * \{
     */
    constexpr basic_iterator() = default;                                   //!< Defaulted.
    constexpr basic_iterator(basic_iterator const &) = default;             //!< Defaulted.
    constexpr basic_iterator(basic_iterator &&) = default;                  //!< Defaulted.
    constexpr basic_iterator & operator=(basic_iterator const &) = default; //!< Defaulted.
    constexpr basic_iterator & operator=(basic_iterator &&) = default;      //!< Defaulted.
    ~basic_iterator() = default;                                            //!< Defaulted.

    //!\brief Allow iterator on a const range to be constructible from an iterator over a non-const range.
    constexpr basic_iterator(basic_iterator<!const_range> const & it) noexcept
        requires const_range
        :
        hash_value{std::move(it.hash_value)},
        roll_factor{std::move(it.roll_factor)},
        shape_{std::move(it.shape_)},
        text_left{std::move(it.text_left)},
        text_right{std::move(it.text_right)}
    {}

    /*!\brief Construct from a given iterator on the text and a seqan3::shape.
    * /param[in] it_start Iterator pointing to the first position of the text.
    * /param[in] it_end   Sentinel pointing to the end of the text.
    * /param[in] s_       The seqan3::shape that determines which positions participate in hashing.
    *
    * \details
    *
    * ### Complexity
    *
    * Linear in size of shape.
    */
    basic_iterator(it_t it_start, sentinel_t it_end, shape s_) :
        shape_{s_},
        text_left{it_start},
        text_right{std::ranges::next(text_left, shape_.size() - 1, it_end)}
    {
        assert(std::ranges::size(shape_) > 0);

        // shape size = 3
        // Text:      1 2 3 4 5 6 7 8 9
        // text_left: ^
        // text_right:    ^
        // distance(text_left, text_right) = 2
        if (shape_.size() <= std::ranges::distance(text_left, text_right) + 1)
        {
            roll_factor = pow(sigma, static_cast<size_t>(std::ranges::size(shape_) - 1));
            hash_full();
        }
    }

    /*!\brief Construct from a given iterator on the text and a seqan3::shape.
    * /param[in] it_start Iterator pointing to the first position of the text.
    * /param[in] it_end   Sentinel pointing to the end of the text.
    * /param[in] s_       The seqan3::shape that determines which positions participate in hashing.
    * /param[in] is_end   Indicates that this iterator should point to the end of the text.
    *
    * \details
    *
    * If we have a common_range as underlying range, we want to preserve this property.
    * This means that we need to have a basic_iterator that can act as end for the kmer_hash_view, i.e.
    * the text_right iterator is equal to the end iterator of the underlying range.
    * However, we still need to do some initialisation via hash_full:
    * When using `std::views::reverse`, we start iterating from the end and decrement the iterator.
    * After calling hash_full, we need to reset our text_right iterator to point to the end again.
    *
    * Another difference to the other constructor is that we need to do some work to determine the position of
    * the text_left iterator. Note that we use `std::ranges::next` instead of `std::ranges::prev` because the latter
    * only works for bidirectional ranges.    *
    *
    * ### Complexity
    *
    * Linear in size of shape.
    */
    basic_iterator(it_t it_start, sentinel_t it_end, shape s_, bool SEQAN3_DOXYGEN_ONLY(is_end)) : shape_{s_}
    {
        assert(std::ranges::size(shape_) > 0);

        auto urange_size = std::ranges::distance(it_start, it_end);
        auto step = (shape_.size() > urange_size + 1) ? 0 : urange_size - shape_.size() + 1;
        text_left = std::ranges::next(it_start, step, it_end);

        // shape size = 3
        // Text:      1 2 3 4 5 6 7 8 9
        // text_left: ^
        // text_right:    ^
        // distance(text_left, text_right) = 2
        if (shape_.size() <= std::ranges::distance(text_left, it_end) + 1)
        {
            roll_factor = pow(sigma, static_cast<size_t>(std::ranges::size(shape_) - 1));
            hash_full();
        }

        text_right = it_end;
    }
    //!\}

    //!\anchor basic_iterator_comparison_kmer_hash
    //!\name Comparison operators
    //!\{

    //!\brief Compare to iterator on text.
    friend bool operator==(basic_iterator const & lhs, sentinel_t const & rhs) noexcept
    {
        return lhs.text_right == rhs;
    }

    //!\brief Compare to iterator on text.
    friend bool operator==(sentinel_t const & lhs, basic_iterator const & rhs) noexcept
    {
        return lhs == rhs.text_right;
    }

    //!\brief Compare to another basic_iterator.
    friend bool operator==(basic_iterator const & lhs, basic_iterator const & rhs) noexcept
    {
        return std::tie(lhs.text_right, lhs.shape_) == std::tie(rhs.text_right, rhs.shape_);
    }

    //!\brief Compare to iterator on text.
    friend bool operator!=(basic_iterator const & lhs, sentinel_t const & rhs) noexcept
    {
        return !(lhs == rhs);
    }

    //!\brief Compare to iterator on text.
    friend bool operator!=(sentinel_t const & lhs, basic_iterator const & rhs) noexcept
    {
        return !(lhs == rhs);
    }

    //!\brief Compare to another basic_iterator.
    friend bool operator!=(basic_iterator const & lhs, basic_iterator const & rhs) noexcept
    {
        return !(lhs == rhs);
    }

    //!\brief Compare to another basic_iterator.
    friend bool operator<(basic_iterator const & lhs, basic_iterator const & rhs) noexcept
    {
        return (lhs.shape_ <= rhs.shape_) && (lhs.text_right < rhs.text_right);
    }

    //!\brief Compare to another basic_iterator.
    friend bool operator>(basic_iterator const & lhs, basic_iterator const & rhs) noexcept
    {
        return (lhs.shape_ >= rhs.shape_) && (lhs.text_right > rhs.text_right);
    }

    //!\brief Compare to another basic_iterator.
    friend bool operator<=(basic_iterator const & lhs, basic_iterator const & rhs) noexcept
    {
        return (lhs.shape_ <= rhs.shape_) && (lhs.text_right <= rhs.text_right);
    }

    //!\brief Compare to another basic_iterator.
    friend bool operator>=(basic_iterator const & lhs, basic_iterator const & rhs) noexcept
    {
        return (lhs.shape_ >= rhs.shape_) && (lhs.text_right >= rhs.text_right);
    }

    //!\}

    //!\brief Pre-increment.
    basic_iterator & operator++() noexcept
    {
        hash_forward();
        return *this;
    }

    //!\brief Post-increment.
    basic_iterator operator++(int) noexcept
    {
        basic_iterator tmp{*this};
        hash_forward();
        return tmp;
    }

    /*!\brief Pre-decrement.
     * \attention This function is only available if `it_t` models std::bidirectional_iterator.
     */
    basic_iterator & operator--() noexcept
        requires std::bidirectional_iterator<it_t>
    {
        hash_backward();
        return *this;
    }

    /*!\brief Post-decrement.
     * \attention This function is only available if `it_t` models std::bidirectional_iterator.
     */
    basic_iterator operator--(int) noexcept
        requires std::bidirectional_iterator<it_t>
    {
        basic_iterator tmp{*this};
        hash_backward();
        return tmp;
    }

    /*!\brief Forward this iterator.
     * \attention This function is only available if `it_t` models std::random_access_iterator.
     */
    basic_iterator & operator+=(difference_type const skip) noexcept
        requires std::random_access_iterator<it_t>
    {
        hash_forward(skip);
        return *this;
    }

    /*!\brief Forward copy of this iterator.
     * \attention This function is only available if `it_t` models std::random_access_iterator.
     */
    basic_iterator operator+(difference_type const skip) const noexcept
        requires std::random_access_iterator<it_t>
    {
        basic_iterator tmp{*this};
        return tmp += skip;
    }

    /*!\brief Non-member operator+ delegates to non-friend operator+.
     * \attention This function is only available if `it_t` models std::random_access_iterator.
     */
    friend basic_iterator operator+(difference_type const skip, basic_iterator const & it) noexcept
        requires std::random_access_iterator<it_t>
    {
        return it + skip;
    }

    /*!\brief Decrement iterator by `skip`.
     * \attention This function is only available if `it_t` models std::random_access_iterator.
     */
    basic_iterator & operator-=(difference_type const skip) noexcept
        requires std::random_access_iterator<it_t>
    {
        hash_backward(skip);
        return *this;
    }

    /*!\anchor basic_iterator_operator-decrement
     * \brief Return decremented copy of this iterator.
     * \attention This function is only available if `it_t` models std::random_access_iterator.
     */
    basic_iterator operator-(difference_type const skip) const noexcept
        requires std::random_access_iterator<it_t>
    {
        basic_iterator tmp{*this};
        return tmp -= skip;
    }

    /*!\brief Non-member operator- delegates to non-friend operator-.
     * \attention This function is only available if `it_t` models std::random_access_iterator.
     */
    friend basic_iterator operator-(difference_type const skip, basic_iterator const & it) noexcept
        requires std::random_access_iterator<it_t>
    {
        return it - skip;
    }

    /*!\anchor basic_iterator_operator-difference
     * \brief Return offset between two iterator's positions.
     * \attention This function is only available if `it_t` models std::random_access_iterator.
     */
    friend difference_type operator-(basic_iterator const & lhs, basic_iterator const & rhs) noexcept
        requires std::sized_sentinel_for<it_t, it_t>
    {
        return static_cast<difference_type>(lhs.text_right - rhs.text_right);
    }

    /*!\brief Return offset between remote sentinel's position and this.
     * \attention This function is only available if sentinel_t and it_t model std::sized_sentinel_for.
     */
    friend difference_type operator-(sentinel_t const & lhs, basic_iterator const & rhs) noexcept
        requires std::sized_sentinel_for<sentinel_t, it_t>
    {
        return static_cast<difference_type>(lhs - rhs.text_right);
    }

    /*!\brief Return offset this and remote sentinel's position.
     * \attention This function is only available if it_t and sentinel_t model std::sized_sentinel_for.
     */
    friend difference_type operator-(basic_iterator const & lhs, sentinel_t const & rhs) noexcept
        requires std::sized_sentinel_for<it_t, sentinel_t>
    {
        return static_cast<difference_type>(lhs.text_right - rhs);
    }

    /*!\brief Move the iterator by a given offset and return the corresponding hash value.
     * \attention This function is only available if `it_t` models std::random_access_iterator.
     */
    reference operator[](difference_type const n) const
        requires std::random_access_iterator<it_t>
    {
        return *(*this + n);
    }

    //!\brief Return the hash value.
    value_type operator*() const noexcept
    {
        return hash_value + to_rank(*text_right);
    }

private:
    //!\brief The alphabet type of the passed iterator.
    using alphabet_t = std::iter_value_t<it_t>;

    //!\brief The alphabet size.
    static constexpr auto const sigma{alphabet_size<alphabet_t>};

    //!\brief The hash value.
    size_t hash_value{0};

    //!\brief The factor for the left most position of the hash value.
    size_t roll_factor{0};

    //!\brief The shape to use.
    shape shape_;

    //!\brief Iterator to the leftmost position of the k-mer.
    it_t text_left;

    //!\brief Iterator to the rightmost position of the k-mer.
    it_t text_right;

    //!\brief Increments iterator by 1.
    void hash_forward()
    {
        if (shape_.all())
        {
            hash_roll_forward();
        }
        else
        {
            std::ranges::advance(text_left, 1);
            hash_full();
        }
    }

    /*!\brief Increments iterator by `skip`.
     * \param skip Amount to increment.
     * \attention This function is only available if `it_t` models std::random_access_iterator.
     */
    void hash_forward(difference_type const skip)
        requires std::random_access_iterator<it_t>
    {
        std::ranges::advance(text_left, skip);
        hash_full();
    }

    /*!\brief Decrements iterator by 1.
     * \attention This function is only available if `it_t` models std::bidirectional_iterator.
     */
    void hash_backward()
        requires std::bidirectional_iterator<it_t>
    {
        if (shape_.all())
        {
            hash_roll_backward();
        }
        else
        {
            std::ranges::advance(text_left, -1);
            hash_full();
        }
    }

    /*!\brief Decrements iterator by `skip`.
     * \param skip Amount to decrement.
     * \attention This function is only available if `it_t` models std::bidirectional_iterator.
     */
    void hash_backward(difference_type const skip)
    {
        std::ranges::advance(text_left, -skip);
        hash_full();
    }

    //!\brief Calculates a hash value by explicitly looking at each position.
    void hash_full()
    {
        text_right = text_left;
        hash_value = 0;

        for (size_t i{0}; i < shape_.size() - 1u; ++i)
        {
            hash_value += shape_[i] * to_rank(*text_right);
            hash_value *= shape_[i] ? sigma : 1;
            std::ranges::advance(text_right, 1);
        }
    }

    //!\brief Calculates the next hash value via rolling hash.
    void hash_roll_forward()
    {
        hash_value -= to_rank(*(text_left)) * roll_factor;
        hash_value += to_rank(*(text_right));
        hash_value *= sigma;

        std::ranges::advance(text_left, 1);
        std::ranges::advance(text_right, 1);
    }

    /*!\brief Calculates the previous hash value via rolling hash.
     * \attention This function is only available if `it_t` models std::bidirectional_iterator.
     */
    void hash_roll_backward()
        requires std::bidirectional_iterator<it_t>
    {
        std::ranges::advance(text_left, -1);
        std::ranges::advance(text_right, -1);

        hash_value /= sigma;
        hash_value -= to_rank(*(text_right));
        hash_value += to_rank(*(text_left)) * roll_factor;
    }
};

//!\brief A deduction guide for the view class template.
template <std::ranges::viewable_range rng_t>
kmer_hash_view(rng_t &&, shape const & shape_) -> kmer_hash_view<std::views::all_t<rng_t>>;

// ---------------------------------------------------------------------------------------------------------------------
// kmer_hash_fn (adaptor definition)
// ---------------------------------------------------------------------------------------------------------------------

//![adaptor_def]
//!\brief views::kmer_hash's range adaptor object type (non-closure).
//!\ingroup search_views
struct kmer_hash_fn
{
    //!\brief Store the shape and return a range adaptor closure object.
    constexpr auto operator()(shape const & shape_) const
    {
        return adaptor_from_functor{*this, shape_};
    }

    /*!\brief            Call the view's constructor with the underlying view and a seqan3::shape as argument.
     * \param[in] urange The input range to process. Must model std::ranges::viewable_range and the reference type
     *                   of the range must model seqan3::semialphabet.
     * \param[in] shape_ The seqan3::shape to use for hashing.
     * \throws std::invalid_argument if resulting hash values would be too big for a 64 bit integer.
     * \returns          A range of converted elements.
     */
    template <std::ranges::range urng_t>
    constexpr auto operator()(urng_t && urange, shape const & shape_) const
    {
        static_assert(std::ranges::viewable_range<urng_t>,
                      "The range parameter to views::kmer_hash cannot be a temporary of a non-view range.");
        static_assert(std::ranges::forward_range<urng_t>,
                      "The range parameter to views::kmer_hash must model std::ranges::forward_range.");
        static_assert(semialphabet<std::ranges::range_reference_t<urng_t>>,
                      "The range parameter to views::kmer_hash must be over elements of seqan3::semialphabet.");

        return kmer_hash_view{std::forward<urng_t>(urange), shape_};
    }
};
//![adaptor_def]

} // namespace seqan3::detail

namespace seqan3::views
{
/*!\brief               Computes hash values for each position of a range via a given shape.
 * \tparam urng_t       The type of the range being processed. See below for requirements. [template parameter is
 *                      omitted in pipe notation]
 * \param[in] urange    The range being processed. [parameter is omitted in pipe notation]
 * \param[in] shape     The seqan3::shape that determines how to compute the hash value.
 * \returns             A range of std::size_t where each value is the hash of the resp. k-mer.
 *                      See below for the properties of the returned range.
 * \ingroup search_views
 *
 * \details
 *
 * \attention
 * For the alphabet size \f$\sigma\f$ of the alphabet of `urange` and the number of 1s \f$s\f$ of `shape` it must hold
 * that \f$s \le \frac{64}{\log_2\sigma}\f$, i.e. hashes resulting from the shape/alphabet combination can be represented
 * in an `uint64_t`.
 *
 * ### View properties
 *
 * | Concepts and traits              | `urng_t` (underlying range type)   | `rrng_t` (returned range type)   |
 * |----------------------------------|:----------------------------------:|:--------------------------------:|
 * | std::ranges::input_range         | *required*                         | *preserved*                      |
 * | std::ranges::forward_range       | *required*                         | *preserved*                      |
 * | std::ranges::bidirectional_range |                                    | *preserved*                      |
 * | std::ranges::random_access_range |                                    | *preserved*                      |
 * | std::ranges::contiguous_range    |                                    | *lost*                           |
 * |                                  |                                    |                                  |
 * | std::ranges::viewable_range      | *required*                         | *guaranteed*                     |
 * | std::ranges::view                |                                    | *guaranteed*                     |
 * | std::ranges::sized_range         |                                    | *preserved*                      |
 * | std::ranges::common_range        |                                    | *preserved*                      |
 * | std::ranges::output_range        |                                    | *lost*                           |
 * | seqan3::const_iterable_range     |                                    | *preserved*                      |
 * |                                  |                                    |                                  |
 * | std::ranges::range_reference_t   | seqan3::semialphabet               | std::size_t                      |
 *
 * See the \link views views submodule documentation \endlink for detailed descriptions of the view properties.
 *
 * \attention The Shape is defined from right to left! The mask 0b11111101 applied to "AGAAAATA" is
 * interpreted as "A.AAAATA" (and not "AGAAAA.A") and will return the hash value for "AAAAATA".
 *
 * ### Example
 *
 * \include test/snippet/search/views/kmer_hash.cpp
 *
 * \hideinitializer
 *
 * \stableapi{Since version 3.1.}
 */
inline constexpr auto kmer_hash = detail::kmer_hash_fn{};

} // namespace seqan3::views
