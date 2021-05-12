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

#define DOCTEST_CONFIG
#include "doctest.h"
#include "sudoku/bitset.hpp"
#include <vector>

#include <type_traits>
#include <algorithm>

static_assert(nb_bits(1) == bits(1));
static_assert(nb_bits(2) == bits(2));
static_assert(nb_bits(3) == bits(2));
static_assert(nb_bits(4) == bits(3));
static_assert(nb_bits(255) == bits(8));
static_assert(nb_bits(256) == bits(9));
static_assert(nb_bits(umax) == max_bits);

static_assert(bytes_req(1) == bytes(1));
static_assert(bytes_req(2) == bytes(1));
static_assert(bytes_req(255) == bytes(1));
static_assert(bytes_req(256) == bytes(2));
static_assert(bytes_req((1UL << 16)-1) == bytes(2));
static_assert(bytes_req(1UL << 16) == bytes(3));

static_assert(std::is_same_v<uint_least_for_t<1>, std::uint_least8_t>);
static_assert(std::is_same_v<uint_least_for_t<255>, std::uint_least8_t>);
static_assert(std::is_same_v<uint_least_for_t<256>, std::uint_least16_t>);
static_assert(std::is_same_v<uint_least_for_t<(1UL<<16)-1>, std::uint_least16_t>);
static_assert(std::is_same_v<uint_least_for_t<1UL<<16>, std::uint_least32_t>);
static_assert(std::is_same_v<uint_least_for_t<(1UL<<32)-1>, std::uint_least32_t>);
static_assert(std::is_same_v<uint_least_for_t<1UL<<32>, std::uint_least64_t>);
static_assert(std::is_same_v<uint_least_for_t<1UL<<42>, std::uint_least64_t>);
static_assert(std::is_same_v<uint_least_for_t<1UL<<63>, std::uint_least64_t>);

static_assert(std::is_same_v<uint_least_for_nbits_t<1>, std::uint_least8_t>);
static_assert(std::is_same_v<uint_least_for_nbits_t<2>, std::uint_least8_t>);
static_assert(std::is_same_v<uint_least_for_nbits_t<3>, std::uint_least8_t>);
static_assert(std::is_same_v<uint_least_for_nbits_t<4>, std::uint_least8_t>);
static_assert(std::is_same_v<uint_least_for_nbits_t<5>, std::uint_least8_t>);
static_assert(std::is_same_v<uint_least_for_nbits_t<6>, std::uint_least8_t>);
static_assert(std::is_same_v<uint_least_for_nbits_t<7>, std::uint_least8_t>);
static_assert(std::is_same_v<uint_least_for_nbits_t<8>, std::uint_least8_t>);

static_assert(std::is_same_v<uint_least_for_nbits_t<9>, std::uint_least16_t>);
static_assert(std::is_same_v<uint_least_for_nbits_t<16>, std::uint_least16_t>);
static_assert(std::is_same_v<uint_least_for_nbits_t<17>, std::uint_least32_t>);
static_assert(std::is_same_v<uint_least_for_nbits_t<32>, std::uint_least32_t>);
static_assert(std::is_same_v<uint_least_for_nbits_t<33>, std::uint_least64_t>);
static_assert(std::is_same_v<uint_least_for_nbits_t<33>, std::uint_least64_t>);

using BS = bitset<bits(9)>;
static_assert(static_cast<unsigned short>(BS(15)) == 15);
static_assert(BS(15).get(0));
static_assert(BS(15).get(1));
static_assert(BS(15).get(2));
static_assert(BS(15).get(3));
static_assert(! BS(15).get(4));
static_assert(! BS(15).get(5));
static_assert(! BS(15).get(6));
static_assert(! BS(15).get(7));
static_assert(! BS(15).get(8));
static_assert(BS(15).popcount() == 4);
static_assert(! BS(15).has_single_bit());
static_assert(BS(4).has_single_bit());

static_assert(BS({0, 2, 4, 6, 8}) == BS(1 | 4 | 16 | 64 | 256));
static_assert(BS({0, 2, 4, 6, 8}).popcount() == 5);

SCENARIO("Test bitset")
{
    GIVEN("An empty bitset<9>")
    {
        BS bs{};
        WHEN("All bits are set")
        {
            bs.set();
            THEN("popcount is 9")
            {
                CHECK(bs.popcount() == 9);
            }
            AND_THEN("there is not a single bit set")
            {
                CHECK(!bs.has_single_bit());
            }
            AND_THEN("all are ones")
            {
                auto const res = {0, 1, 2, 3, 4, 5, 6, 7, 8};
                std::vector<std::uint8_t> v0;
                for (auto o : bs.ones()) {
                    v0.push_back(o);
                }

                CHECK(std::equal(std::begin(res), std::end(res), v0.begin(), v0.end()));

#if 0
                auto ones = bs.ones();
                // needs a stdlib w/ std::ranges::equal
                CHECK(std::ranges::equal(std::begin(res), std::end(res), ones.begin(), ones.end()));
#endif
                CHECK(std::equal(std::begin(res), std::end(res), std::begin(bs), std::end(bs)));
            }

            AND_WHEN("3th bit is removed")
            {
                bs.reset(2);
                THEN("popcount is 8")
                {
                    CHECK(bs.popcount() == 8);
                }
                AND_THEN("there is not a single bit set")
                {
                    CHECK(!bs.has_single_bit());
                }
                AND_THEN("int value is 0b111111011")
                {
                    CHECK(bs.as_int() == 0b111111011);
                }

                AND_WHEN("All bits but 9th are removed")
                {
                    bs.reset(0, 1, 2, 3, 4, 5, 6, 7);
                    THEN("popcount is 1")
                    {
                        CHECK(bs.popcount() == 1);
                    }
                    AND_THEN("there is a single bit")
                    {
                        CHECK(bs.has_single_bit());
                    }
                    AND_THEN("int value is 2^9")
                    {
                        CHECK(bs.as_int() == 1UL << 8);
                    }
                }
            }

            AND_WHEN("it's masked with 0b110011001")
            {
                auto const mask = 0b110011001;
                BS b2(mask);
                CHECK(b2.as_int() == mask);
                bs &= b2;
                THEN("it values the mask")
                {
                    CHECK(bs == b2);
                }
                AND_THEN("we can iterate with coroutines")
                {
                    auto const ref = {0, 3, 4, 7, 8};
                    auto ones = bs.ones();
                    std::vector<std::uint8_t> v;
                    for (auto o : ones) {
                        v.push_back(o);
                    }
                    CHECK(std::equal(std::begin(v), std::end(v), std::begin(ref), std::end(ref)));
                }
                AND_THEN("we can iterate with begin/end")
                {
                    auto const ref = {0, 3, 4, 7, 8};
                    CHECK(std::equal(std::begin(bs), std::end(bs), std::begin(ref), std::end(ref)));
                }
            }
        }
    }
}
