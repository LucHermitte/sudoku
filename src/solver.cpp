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

#include "sudoku/solver.hpp"
#include <optional>

std::pair<grid::index_type, grid::index_type>
next_unsolved_cell(grid const& gr, grid::index_type l, grid::index_type c) noexcept
{
    for (std::size_t i = std::exchange(l, 0) ; i < 9 ; ++i) {
        for (std::size_t j = std::exchange(c, 0) ; j < 9 ; ++j) {
            // std::cout << "check: " << i << ", " << j << " --> " << gr(i,j) <<"\n";
            if (! gr(i,j).has_single_value())
            {
                return {i, j};
            }
        }
    }
    return {9,9};
}

std::optional<grid> brute_force(grid gr, std::size_t i, std::size_t j, std::uint_least16_t value, int & nb_explored)
{
    ++nb_explored;
    gr.set(i, j, value);
    if (gr.is_solved()) {
        return gr;
    }
    else if (! gr.is_valid()) {
        return std::nullopt;
    }
    else {
        auto [ni, nj] = next_unsolved_cell(gr, i, j);
        auto const& cel = gr(ni, nj);
        for (auto bv : cel) {
            auto const value = bv + 1;
            // std::cout << "- " << int(value) << "\n";
            assert(1 <= value);
            assert(value <= 9);
            if (auto br = brute_force(gr, ni, nj, value, nb_explored); br) {
                return br;
            }
        }
        return std::nullopt;
    }
}

std::optional<grid> brute_force(grid gr, int& nb_explored)
{
    auto [ni, nj] = next_unsolved_cell(gr, 0, 0);
    // std::cout << "next: " << int(ni) << ", " << int(nj) << "\n";
    if (ni == 9 && nj == 9) {
        if (gr.is_solved()) {
            return gr;
        }
        else {
            return std::nullopt;
        }
    }
    auto const& cel = gr(ni, nj);
    for (auto bv : cel) {
        auto const value = bv + 1;
        // std::cout << "- " << int(value) << "\n";
        assert(1 <= value);
        assert(value <= 9);
        if (auto br = brute_force(gr, ni, nj, value, nb_explored); br) {
            return br;
        }
    }
    return std::nullopt;
}
