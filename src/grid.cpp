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

#include "sudoku/grid.hpp"
#include <istream>
#include <ostream>
#include <iomanip>
#include <sstream>

namespace
{ // Anonymous namespace

auto remove(any_group auto group, std::uint8_t value) {
    assert(1 <= value && value <= 9);
    // assert(is_valid(group));
    std::array<bool, 9> changes{};
    for (int i = 0; i < 9; ++i) {
        auto && cel = at(group, i);
        static_assert(std::is_lvalue_reference_v<decltype(cel)>);
        changes[i] = cel.remove(value);
    }
    // assert(is_valid(group));
    return changes;
}
} // Anonymous namespace


std::ostream & operator<<(std::ostream & os, cell const& v)
{
    if (v.has_single_value()) {
        os << '(' << v.value() << ')';
    }
    else {
        os << std::setw(3) << v.as_int();
    }
    return os;
}

std::ostream & operator<<(std::ostream & os, grid const& v)
{
    if (v.is_solved()) {
        for (grid::index_type i = 0 ; i < 9 ; ++i) {
            if (i == 3 || i == 6) {
                os << "---+---+---\n";
            }
            auto line = const_cast<grid&>(v).line(i);
            for (grid::index_type j = 0 ; j < 9 ; ++j) {
                if (j == 3 || j == 6) {
                    os << '|';
                }
                os << line(j).value();
            }
            os << '\n';
        }
    }
    else {
        os << "[[\n";
        for (grid::index_type i = 0 ; i < 9 ; ++i) {
            auto line = const_cast<grid&>(v).line(i);
            for (grid::index_type j = 0 ; j < 9 ; ++j) {
                os << line(j);
                os << ", ";
            }
            os << "\n";
        }
        os << "]]\n";
    }
    return os;
}

std::istream & grid::read(std::istream & is)
{
    grid_t gr = m_grid;
    std::string li;
    for (std::size_t i = 0 ; i < 9 && std::getline(is, li); ) {
        if (li.find_first_of("0123456789.") == std::string::npos) {
            continue;
        }
        // std::cout << "line: " << li << std::endl;
        for (std::size_t p = 0, j = 0; p < size(li) && j < 9; ++p) {
            if ('1' <= li[p] && li[p] <= '9') {
                auto v = li[p] - '0';
                set(i, j, v);
            }
            if (li[p] != '|') {
                j++;
            }
        }
        i++;
    }
    if (!is) {
        gr = m_grid;
    }
    return is;
}

void grid::set(grid::index_type l, grid::index_type c, std::uint8_t value) noexcept
{
    assert(l < 9);
    assert(c < 9);
    assert(1 <= value && value <= 9);

    auto gl = line(l);
    remove(gl, value);

    auto gc = column(c);
    remove(gc, value);

    auto gb = box(l, c);
    // TODO: Optimize the removal for boxes as 4/9 cells have already been
    // updated and the value of the last one will be overridden.
    remove(gb, value);

    auto && cel = at(gl, c);
    cel.set(value);

    assert(&at(gl, c) == &at(gc, l));
    auto pl = &at(gl, c);
    auto pb = &at(gb, 3*(l%3)+c%3);
    assert(pl == pb);
    assert(&at(gl, c) == &at(gb, 3*(l%3)+c%3));

    assert(at(gl, c).has_single_value());
    assert(at(gl, c).value() == value);
    assert(at(gc, l).has_single_value());
    assert(at(gc, l).value() == value);
    assert(at(gb, 3*(l%3)+c%3).has_single_value());
    assert(at(gb, 3*(l%3)+c%3).value() == value);
}

std::variant<grid::is_ok, grid::has_bad_line, grid::has_bad_column, grid::has_bad_box>
grid::validity() const noexcept
{
    auto & th = const_cast<grid&>(*this); // I can't make line() const ...
    for (index_type i = 0 ; i < 9 ; ++i) {
        if (!::is_valid(th.line(i)))   return has_bad_line{i};
        if (!::is_valid(th.column(i))) return has_bad_column{i};
        if (!::is_valid(th.box(i)))    return has_bad_box{i};
    }
    return is_ok{};
}

bool grid::is_solved() const noexcept
{
    auto & th = const_cast<grid&>(*this); // I can't make line() const ...
    for (index_type i = 0 ; i < 9 ; ++i) {
        bool const solved = ::is_solved(th.line(i))
            && ::is_solved(th.column(i))
            && ::is_solved(th.box(i));
        if (!solved) {
            return false;
        }
    }
    return true;
}
