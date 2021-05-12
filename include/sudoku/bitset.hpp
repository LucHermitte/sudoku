//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or(at
//  your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http: //www.gnu.org/licenses/>.

#ifndef BITSET_HPP__
#define BITSET_HPP__

#include <cstdint>
#include <climits>
#include <limits>
#include <bit>
#include <type_traits>
#include <cassert>
#include <concepts>
#include <initializer_list>
#include <compare>
#include <algorithm>
#include <ostream>
#include <cppcoro/generator.hpp>
#include <iostream> // debug

enum class bits : std::uintmax_t {};
enum class bytes : std::uintmax_t {};

constexpr auto to_underlying = [](auto e) noexcept {
    return static_cast<std::underlying_type_t<decltype(e)>>(e);
};

constexpr bits operator+(bits a, bits b) noexcept {
    return bits(to_underlying(a) + to_underlying(b));
}

constexpr bits operator-(bits a, auto b) noexcept {
    return bits(to_underlying(a) - b);
}

constexpr bytes operator+(auto a, bytes b) noexcept {
    return bytes(a + to_underlying(b));
}

constexpr bytes operator+(bytes a, bytes b) noexcept {
    return bytes(to_underlying(a) + to_underlying(b));
}

constexpr bits in_bits(bytes a) noexcept {
    return bits(to_underlying(a) * CHAR_BIT);
}

constexpr bytes in_bytes(bits a) noexcept {
    return bytes(to_underlying(a) / CHAR_BIT);
}

constexpr auto umax = std::numeric_limits<std::uintmax_t>::max();
constexpr bits max_bits = bits(sizeof(umax) * CHAR_BIT);
constexpr bits nb_bits(std::uintmax_t N) noexcept {
    unsigned int res = 1;
    while(res < to_underlying(max_bits) && (1ULL<<res) <= N)
    { ++res; }
    return bits(res);
}

template <bytes N> struct uint_least_for_bytes { };

template <> struct uint_least_for_bytes<bytes(1)> { using type = std::uint_least8_t; };
template <> struct uint_least_for_bytes<bytes(2)> { using type = std::uint_least16_t; };
template <> struct uint_least_for_bytes<bytes(4)> { using type = std::uint_least32_t; };
template <> struct uint_least_for_bytes<bytes(8)> { using type = std::uint_least64_t; };

constexpr bytes bytes_req(bits           b) noexcept { return 1+in_bytes(b-1); }
constexpr bytes bytes_req(std::uintmax_t N) noexcept { return bytes_req(nb_bits(N)); }

constexpr bytes ceil_bytes_req(auto N) noexcept {
#if defined(OLD_BIT_LIB_API)
    // as <bit> exist, let's bet we have the pre-c++20 std::ceil2 function
    // instead of std::bit_ceil
    return bytes(std::ceil2(to_underlying(bytes_req(N))));
#else
    return bytes(std::bit_ceil(to_underlying(bytes_req(N))));
#endif
}

template <uintmax_t N>
using uint_least_for_t = typename uint_least_for_bytes<ceil_bytes_req(N)>::type;

template <uintmax_t B>
using uint_least_for_nbits_t = typename uint_least_for_bytes<ceil_bytes_req(bits(B))>::type;

template <std::unsigned_integral UT>
// template <typename UT>
constexpr UT set_nth_bits(bits b) noexcept
{
    std::uint8_t const N = to_underlying(b);
    UT res{};
    for (std::uint8_t i = 0U; i < N ; ++i) {
        res |= 1U << i;
    }
    return res;
}

template <bits B>
class bitset
{
    using word_type = uint_least_for_nbits_t<to_underlying(B)>;
    static constexpr auto mask = set_nth_bits<word_type>(B);
public:
    using index_type = std::uint8_t; // no more than 64 bits for now

    constexpr bitset() noexcept = default;
    constexpr bitset(word_type w) noexcept : m_buffer{w}
    { check_invariant(); }

    constexpr bitset(std::initializer_list<index_type> bs) noexcept {
        set(bs);
    }

    constexpr bool get(index_type i) const noexcept {
        assert(i < to_underlying(B));
        word_type r = m_buffer & (1ULL << i);
        return r != 0;
    }

    constexpr void set() noexcept
    {
        m_buffer = ~0ULL & mask;
        check_invariant();
    }
    constexpr void set(std::initializer_list<index_type> bs) noexcept {
        // loop is optimized away
        word_type m {};
        for (auto b : bs) {
            assert(b < to_underlying(B));
            m |= 1ULL << b;
        }
        m_buffer |= m;
        check_invariant();
    }
    constexpr void set(auto... bs) noexcept {
        m_buffer |= ((1ULL << bs) | ...);
        check_invariant();
    }

    constexpr void reset() noexcept
    {
        m_buffer = 0ULL;
        check_invariant();
    }
    constexpr void reset(std::initializer_list<index_type> bs) noexcept {
        // loop is optimized away
        word_type m {};
        for (auto b : bs) {
            assert(b < to_underlying(B));
            m |= 1ULL << b;
        }
        m_buffer &= (~m & mask);
        check_invariant();
    }
    constexpr void reset(auto... bs) noexcept {
        m_buffer &= ~((1ULL << bs) | ...) & mask;
        check_invariant();
    }

    constexpr bitset& operator|=(bitset const& b) noexcept
    { m_buffer |= b.m_buffer; check_invariant(); return *this; }

    constexpr bitset& operator&=(bitset const& b) noexcept
    { m_buffer &= b.m_buffer; check_invariant(); return *this; }

    constexpr bitset& operator^=(bitset const& b) noexcept
    { m_buffer ^= b.m_buffer; check_invariant(); return *this; }

    constexpr bitset operator~() const noexcept
    { auto r = *this; r.flip(); check_invariant(); return r; }

    constexpr bitset& flip() noexcept
    { m_buffer = ~m_buffer & mask; check_invariant(); return *this; }

    constexpr auto operator<=>(bitset const& rhs) const noexcept = default;

    friend constexpr bitset operator|(bitset const& a, bitset const& b) noexcept
    { a |= b; return a; }

    friend constexpr bitset operator&(bitset const& a, bitset const& b) noexcept
    { a &= b; return a; }

    friend constexpr bitset operator^(bitset const& a, bitset const& b) noexcept
    { a ^= b; return a; }

    // TODO: only if only one word
    explicit constexpr operator word_type() const noexcept { return m_buffer; }
    constexpr word_type as_int() const noexcept { return m_buffer; }

    /// number of ones
    constexpr auto popcount() const noexcept { return std::popcount(m_buffer); }

#if defined(OLD_BIT_LIB_API)
    constexpr bool has_single_bit() const noexcept { return std::ispow2(m_buffer); }
#else
    constexpr bool has_single_bit() const noexcept { return std::has_single_bit(m_buffer); }
#endif

    friend std::ostream & operator<<(std::ostream & os, bitset const& v)
    {
        return os << v.m_buffer;
    }

    // Cannot be used with normal algorithms, or container constructors
    // Needs std::ranges:: algorithms and generator<>::end() is not an
    // iterator but a sentinel type.
    cppcoro::generator<index_type const> ones()
    {
        index_type const N = to_underlying(B);
        for (index_type b = 0 ; b < N ; ++b) {
            if (m_buffer & (1ULL << b)) {
                co_yield b;
            }
        }
    }

    struct const_iterator {
        using iterator_category = std::input_iterator_tag;
        using value_type        = bool;
        using difference_type   = std::ptrdiff_t;
        using reference         = void;
        using pointer           = void;
        constexpr const_iterator(bitset const& bs) noexcept
            : const_iterator(bs, std::min<index_type>(to_underlying(B), std::countr_zero(bs.as_int())))
            {}
        constexpr const_iterator(bitset const& bs, index_type idx) noexcept
        : m_bs(bs), m_idx(idx) { }
        constexpr const_iterator& operator++() noexcept{
            assert(m_idx < to_underlying(B));
            while ( ++m_idx < to_underlying(B) && !m_bs.get(m_idx) )
            {
                // INFO("#"<<m_idx<<" -> " << m_bs.get(m_idx));
            }
            return *this;
        }
        constexpr decltype(auto) operator*() const noexcept
        { assert(m_idx < to_underlying(B)); assert(m_bs.get(m_idx)); return m_idx;}
        friend constexpr bool operator==(const_iterator const& a, const_iterator const& b) noexcept
        { assert(&a.m_bs == &b.m_bs); return a.m_idx == b.m_idx; }
        friend constexpr bool operator!=(const_iterator const& a, const_iterator const& b) noexcept
        { return !(a==b); }
    private:
        bitset const& m_bs;
        index_type    m_idx = 0;
    };
    auto begin() const noexcept { return const_iterator(*this); }
    auto end  () const noexcept { return const_iterator(*this, to_underlying(B)); }
private:
    constexpr void check_invariant() noexcept
    { assert(m_buffer <= mask); }

    // TODO: if nb words required > 1 => buffer is an array<word_type>
    using buffer_type = word_type;
    // Because of the invariant, default constructor cannot leave the bitset
    // in an unknown state
    buffer_type m_buffer {};
};

#endif  // BITSET_HPP__
