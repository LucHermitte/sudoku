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
#include "sudoku/grid.hpp"
#include <ostream>
#include <sstream>

namespace std
{
std::ostream& operator<<(std::ostream & os, std::array<bool, 9> const& v)
{
    os << "[" << std::boolalpha;
    for (auto b : v) {
        os << b << ", ";
    }
    os << "]";
    return os;
}
}


void test_changing_group_manually(any_group auto & group)
{
    CHECK(is_valid(group));
    for (int i=0; i<9 ; ++i) {
        CAPTURE(i);
        at(group, i).set(i+1);
        // group[i].set(i+1);
        CHECK(is_valid(group));
    }
}

void test_changing_group_globally(any_group auto & group)
{
    CHECK(is_valid(group));
    for (int i=0; i<9 ; ++i) {
        CAPTURE(i);
        auto changes = set(group, i, i+1);
        CAPTURE(changes);
        CHECK(is_valid(group));
        REQUIRE(at(group, i).has_single_value());
        REQUIRE(at(group, i).value() == i+1);
        for (int j=0; j <= i ; ++j) {
            CAPTURE(j);
            auto group_j = at(group, j);
            REQUIRE(group_j.has_single_value());
            CHECK(group_j.value() == j+1);
        }
        for (int j=i+1; j < 9 ; ++j) {
            CAPTURE(j);
            auto group_j = at(group, j);
            auto const raw = group_j.as_int();
            CHECK(std::countr_zero(raw) == i+1);
            CHECK((raw & 0b1111'1110'0000'0000) == 0);
            auto mask = 0b0000'0001'1111'1111 ;
            auto expected = mask & ~((1 << (i+1)) - 1);
            CHECK(raw == expected);
        }
    }

}

SCENARIO("Test valid groups")
{
    GIVEN("A raw grid")
    {
        grid gr;
        CAPTURE(gr);
        // auto line = subspan(grid.view(), 0, std::pair{0,8});

        WHEN("Changing a line manually")
        {
            auto line = gr.line(0);
            static_assert(line.rank() == 1);
            static_assert(grid::line_type::rank() == 1);
            static_assert(is_group_v<grid::line_type>);
            static_assert(is_rk1_group_v<grid::line_type>);
            auto p1 = at(line, 0);
            THEN("It stays valid")
            {
                CHECK(p1.as_int() == (1U<<9)-1);
                test_changing_group_manually(line);
            }
        }
        WHEN("Changing a column manually")
        {
            auto col = gr.column(0);
            static_assert(col.rank() == 1);
            static_assert(grid::column_type::rank() == 1);
            static_assert(is_rk1_group_v<grid::column_type>);
            auto p1 = at(col, 0);
            THEN("It stays valid")
            {
                CHECK(p1.as_int() == (1U<<9)-1);
                test_changing_group_manually(col);
            }
        }
        WHEN("Changing a box manually")
        {
            auto box = gr.box(0);
            static_assert(box.rank() == 2);
            static_assert(grid::box_type::rank() == 2);
            static_assert(is_rk2_group_v<grid::box_type>);
            auto p1 = at(box, 8);
            auto &pr = at(box, 8);
            THEN("It stays valid")
            {
                CHECK(p1.as_int() == (1U<<9)-1);
                CHECK(at(box, 8).as_int() == p1.as_int());
                CHECK(&at(box, 8) == &pr);
                test_changing_group_manually(box);
                // CHECK(false);
            }
        }

        WHEN("Changing a line globally")
        {
            auto line = gr.line(0);
            THEN("Candidates are updated")
            {
                test_changing_group_globally(line);
            }
        }
        WHEN("Changing a column globally")
        {
            auto col = gr.column(0);
            THEN("Candidates are updated")
            {
                test_changing_group_globally(col);
            }
        }
        WHEN("Changing a box globally")
        {
            auto box = gr.box(0);
            THEN("Candidates are updated")
            {
                test_changing_group_globally(box);
            }
        }
    }
}

SCENARIO("Read a grid from stream")
{
    GIVEN("a grid")
    {
        grid gr;
        CAPTURE(gr);
        WHEN("reading a grid from a stream")
        {
            std::string def =
                "8.. ... ...\n"
                "..3 6.. ...\n"
                ".7. .9. 2..\n"
                "\n"
                ".5. ..7 ...\n"
                "... .45 7..\n"
                "... 1.. .3.\n"
                "\n"
                "..1 ... .68\n"
                "..8 5.. .1.\n"
                ".9. ... 4..\n";
            std::istringstream iss(def);
            auto &r = gr.read(iss);
            THEN("the reading succeeds")
            {
                REQUIRE(r);
                REQUIRE(gr.is_valid());
            }
            AND_THEN("None of the lines is solved")
            {
                for (int i = 0 ; i < 9 ; ++i) {
                    CAPTURE(i);
                    CHECK(! is_solved(gr.line(i)));
                }
            }
            AND_THEN("None of the columns is solved")
            {
                for (int i = 0 ; i < 9 ; ++i) {
                    CAPTURE(i);
                    CHECK(! is_solved(gr.column(i)));
                }
            }
            AND_THEN("None of the boxes is solved")
            {
                for (int i = 0 ; i < 9 ; ++i) {
                    CAPTURE(i);
                    CHECK(! is_solved(gr.box(i)));
                }
            }
        }
    }
}

SCENARIO("Test on a solved grid")
{
    GIVEN("a solved grid")
    {
        grid gr;
        CAPTURE(gr);
        WHEN("reading a solved grid from a stream")
        {
            std::string def =
                "812 753 649\n"
                "943 682 175\n"
                "675 491 283\n"
                "\n"
                "154 237 896\n"
                "369 845 721\n"
                "287 169 534\n"
                "\n"
                "521 974 368\n"
                "438 526 917\n"
                "796 318 452\n";
            std::istringstream iss(def);
            auto &r = gr.read(iss);
            THEN("the reading succeeds")
            {
                REQUIRE(r);
                REQUIRE(gr.is_valid());
            }
            AND_THEN("All of the lines are solved")
            {
                for (int i = 0 ; i < 9 ; ++i) {
                    CAPTURE(i);
                    CHECK(is_solved(gr.line(i)));
                }
            }
            AND_THEN("All of the columns are solved")
            {
                for (int i = 0 ; i < 9 ; ++i) {
                    CAPTURE(i);
                    CHECK(is_solved(gr.column(i)));
                }
            }
            AND_THEN("All of the boxes are solved")
            {
                for (int i = 0 ; i < 9 ; ++i) {
                    CAPTURE(i);
                    CHECK(is_solved(gr.box(i)));
                }
            }
        }
    }
}
