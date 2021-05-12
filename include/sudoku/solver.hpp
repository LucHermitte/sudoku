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

#ifndef SOLVER_HPP__
#define SOLVER_HPP__

#include "sudoku/grid.hpp"
#include <optional>

// std::optional<grid> brute_force(grid gr, std::size_t i, std::size_t j, std::uint_least16_t value);
std::optional<grid> brute_force(grid gr, int& nb_explored);


#endif  // SOLVER_HPP__
