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

#ifndef SUDOKU_GRID_HPP__
#define SUDOKU_GRID_HPP__

#include "sudoku/cell.hpp"
#include <experimental/mdarray>
#include <experimental/mdspan>
#include <array>
#include <algorithm>
#include <iosfwd>
#include <variant>
#include <ostream>

namespace stdex = std::experimental;

class grid
{
    using grid_t = stdex::mdarray<cell, 9, 9>;
public:
    using index_type = std::uint8_t;
    auto line(index_type idx) noexcept {
        assert(idx < 9);
        return subspan(m_grid.view(), idx, stdex::all);
    }
#if 0
    // Doesn't compile. Bug in subspan?
    auto line(index_type idx) const noexcept {
        assert(idx < 9);
        return subspan(m_grid.view(), idx, stdex::all);
    }
#endif
    auto column(index_type idx) noexcept {
        assert(idx < 9);
        return subspan(m_grid.view(), stdex::all, idx);
    }
    auto box(index_type idx) noexcept {
        assert(idx < 9);
        auto const li = 3 * (idx / 3);
        auto const co = 3 * (idx % 3);
        assert(li+3 <= 9);
        assert(li+3 <= 9);
        return subspan(m_grid.view(), std::pair{li, li+3}, std::pair{co, co+3});
    }
    auto box(index_type l, index_type c) noexcept {
        return box(3*(l/3) + c/3);
    }

    // The actual type is quite complex:
    // --> stdex::basic_mdspan<cell, stdex::extents<9>, stdex::layout_right, stdex::__mdarray_version_0::array_container_policy<cell, 81> >
    using line_type   = decltype(subspan(std::declval<grid_t>().view(), 0, stdex::all));
    using column_type = decltype(subspan(std::declval<grid_t>().view(), stdex::all, 0));
    using box_type    = decltype(subspan(std::declval<grid_t>().view(), std::pair{0, 3}, std::pair{0, 3}));

    std::istream & read(std::istream & is);

    friend std::ostream & operator<<(std::ostream & os, grid const& v);

    decltype(auto) operator()(std::size_t i, std::size_t j) const noexcept {
        assert(i < 9);
        assert(j < 9);
        return m_grid(i, j);
    }

    void set(index_type l, index_type c, std::uint8_t value) noexcept;

    struct is_ok {
        auto operator<=>(is_ok const&) const noexcept = default;
    };
    enum class has_bad_line   : std::uint8_t {};
    enum class has_bad_column : std::uint8_t {};
    enum class has_bad_box    : std::uint8_t {};
    std::variant<is_ok, has_bad_line, has_bad_column, has_bad_box> validity() const noexcept;

    bool is_valid() const noexcept {
        return std::holds_alternative<is_ok>(validity());
    }

    bool is_solved() const noexcept;

private:
    grid_t m_grid;
};

// using group_t = stdex::mdspan<cell, stdex::dynamic_extent, stdex::dynamic_extent>;


template <typename T> struct is_group          : std::false_type {};
template <> struct is_group<grid::line_type>   : std::true_type {};
template <> struct is_group<grid::column_type> : std::true_type {};
template <> struct is_group<grid::box_type>    : std::true_type {};

template <typename T>
inline constexpr bool is_group_v = is_group<std::remove_cvref_t<T>>::value;

#if 1
template <typename T> struct is_rk1_group          : std::false_type {};
template <> struct is_rk1_group<grid::line_type>   : std::true_type {};
template <> struct is_rk1_group<grid::column_type> : std::true_type {};

template <typename T>
inline constexpr bool is_rk1_group_v = is_rk1_group<std::remove_cvref_t<T>>::value;

template <typename T> struct is_rk2_group          : std::false_type {};
template <> struct is_rk2_group<grid::box_type>    : std::true_type {};

template <typename T>
inline constexpr bool is_rk2_group_v = is_rk2_group<std::remove_cvref_t<T>>::value;
#else
// Can't make it work with `any_rk1_group const&`
template <typename T>
inline constexpr bool is_rk1_group_v = is_group<T>::value && (std::remove_cvref_t<T>::rank() == 1);

template <typename T>
inline constexpr bool is_rk2_group_v = is_group<T>::value && (std::remove_cvref_t<T>::rank() == 2);
#endif

template <typename T>
concept any_group = is_group_v<T>;

template <typename T>
concept any_rk1_group = is_rk1_group_v<T>;

template <typename T>
concept any_rk2_group = is_rk2_group_v<T>;

decltype(auto) at(any_rk1_group auto group, std::size_t p) noexcept {
    assert(p < 9);
    return group(p);
}

decltype(auto) at(any_rk2_group auto group, std::size_t p) noexcept {
    assert(p < 9);
    return group(p/3, p%3);
}


#if 1
inline
std::ostream& operator<<(std::ostream& os, std::array<unsigned, 9> const& a) {
    os << "[";
    for (auto i : a)  os << i << ", ";
    return os << "]";
}
#endif

bool is_solved(any_group auto group)
{
    static_assert(is_group_v<decltype(group)>);
    std::array<unsigned int, 9> nb{};
    // std::cout << "CELLS: {";
    for (int i = 0; i < 9; ++i) {
        auto const& cel = at(group, i);
        if (! cel.has_single_value())
            return false;
        ++nb[cel.value()-1]; // not noexcept...
        // std::cout << ", " << cel.value();
    }
    // std::cout << "}\nNB: " << nb << "\n";
    bool const ok = std::all_of(std::begin(nb), std::end(nb), [](int n){return n==1;});

    return ok;
}

struct validity_state {
    std::array<unsigned int, 9> nb_occurences;
    std::array<unsigned int, 9> nb_unique_occurences;

    bool are_all_represented() const noexcept {
        return std::all_of(std::begin(nb_occurences), std::end(nb_occurences), [](int n){return n>=1;});
    }
    bool are_there_no_duplicated() const noexcept {
        return std::all_of(std::begin(nb_unique_occurences), std::end(nb_unique_occurences), [](int n){return n<=1;});;
    }
    bool is_valid() const noexcept {
        return are_all_represented() && are_there_no_duplicated();
    }

    friend std::ostream& operator<<(std::ostream & os, validity_state const& v);
};

validity_state validity(any_group auto group)
{
    static_assert(is_group_v<decltype(group)>);
    std::array<unsigned int, 9> nb{};
    std::array<unsigned int, 9> nb_for_uniq{};
    for (int i = 0; i < 9; ++i) {
        auto const& cel = at(group, i);
        auto const has_single_value = cel.has_single_value();
        for (auto idx : cel) {
            ++nb[idx]; // not noexcept...
            if (has_single_value) {
                ++nb_for_uniq[idx]; // not noexcept...
            }
        }
    }
    return validity_state{nb, nb_for_uniq};
}

bool is_valid(any_group auto group)
{
    static_assert(is_group_v<decltype(group)>);
    std::array<unsigned int, 9> nb{};
    std::array<unsigned int, 9> nb_for_uniq{};
    for (int i = 0; i < 9; ++i) {
        auto const& cel = at(group, i);
        auto const has_single_value = cel.has_single_value();
        for (auto idx : cel) {
            ++nb[idx]; // not noexcept...
            if (has_single_value) {
                ++nb_for_uniq[idx]; // not noexcept...
            }
        }
    }
    bool const all_are_represented = std::all_of(std::begin(nb), std::end(nb), [](int n){return n>=1;});
    bool const there_is_no_duplicated = std::all_of(std::begin(nb_for_uniq), std::end(nb_for_uniq), [](int n){return n<=1;});

    return all_are_represented && there_is_no_duplicated;
}

auto set(any_group auto group, grid::index_type idx, std::uint8_t value) {
    assert(idx < 9);
    assert(1 <= value && value <= 9);
    assert(is_valid(group));
    std::array<bool, 9> changes{};
    for (int i = 0; i < 9; ++i) {
        // auto & cel = group(i);
        auto && cel = at(group, i);
        static_assert(std::is_lvalue_reference_v<decltype(cel)>);
        if (i == idx) {
            changes[i] = cel.set(value);
            assert(at(group, i).has_single_value());
            assert(at(group, i).value() == value);
        } else {
            changes[i] = cel.remove(value);
        }
    }
    assert(is_valid(group));
    return changes;
}

struct validity_inspector {
    explicit validity_inspector(grid const& g) : gr(g) {}

    friend std::ostream& operator<<(std::ostream & os, validity_inspector const& vi);

private:
    grid const& gr;
};

#endif  // SUDOKU_GRID_HPP__
