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

#ifndef SUDOKU_CELL_HPP__
#define SUDOKU_CELL_HPP__

#include "sudoku/bitset.hpp"
#include <ostream>

class cell
{
    using cell_t = bitset<bits(9)>;

public:
    using index_type = cell_t::index_type;

    constexpr cell() noexcept { m_cell.set(); }

    constexpr bool set(index_type i) noexcept {
        assert(1<=i && i <= 9);
        auto old_value = m_cell;
        m_cell = cell_t{--i}; /* init-list ctr*/
        return m_cell != old_value;
    }

    constexpr bool remove(auto... bs) noexcept {
        assert(((1<=bs && bs <= 9) && ...));
        auto old_value = m_cell;
        m_cell.reset((bs-1)...);
        return old_value != m_cell; // true if removing changes anything
    }

    constexpr auto as_int() const noexcept { return m_cell.as_int(); }
    constexpr bool has_single_value() const noexcept { return m_cell.has_single_bit(); }

    // Iterate over candidates/single value
    constexpr auto begin() const noexcept { return m_cell.begin(); }
    constexpr auto end  () const noexcept { return m_cell.end  (); }

    constexpr auto value() const noexcept {
        assert(has_single_value());
        return 1 + std::countr_zero(as_int());
    }

    friend std::ostream & operator<<(std::ostream & os, cell const& v);
private:
    cell_t m_cell;
};

#endif  // SUDOKU_CELL_HPP__
