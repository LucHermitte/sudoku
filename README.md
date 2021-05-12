# Yet another sodoku solver

This is a modest proof of concept where I intended to play with advanced human
heuristics, and also with C++20, C++23 `stdex::mdspan` and `stdex::mdarray`,
_Modern CMake_, Doctest BDD macros... Hence this overdesigned solution.

In the end, brute force solving is extremely fast: we don't really need
heuristics like _hidden singles_, _naked pairs/triples_, _pointing_...

Yet the internals has been designed with these evolutions in mind. Hence the
internal (optimized) `bitset` to model candidates in cells.

Many things remain to be done like

- cleaning dead code
- documenting some classes and design choices
- simplify the cells
- implement these human solving heuristics (see
  http://www.sudokusnake.com/techniques.php,
  http://angusj.com/sudoku/hints.php, etc)

The input format should be compatible with the puzzles available on
[Simple Sudoku website](http://www.angusj.com/sudoku/).


## Install
To compile the code you'll need

- a C++20 compiler like clang++ 13 for instance
- CMake 3.19

Then, the compilation can be done with:

```bash
# Configure in ../build/build-static-13
CXX=clang++ cmake -G Ninja -S . -B ../build/build-static-13 -DBUILD_SHARED_LIBS=NO  -DCMAKE_BUILD_TYPE=Release  -DCMAKE_CXX_FLAGS='-stdlib=libc++ -Wall -Wextra' -DCMAKE_EXE_LINKER_FLAGS='-stdlib=libc++' -DCMAKE_EXPORT_COMPILE_COMMANDS=On

# Compile
cmake --build ../build/build-static-13
```

Tests can be ran with

```bash
cd ../build/build-static-13 && ctest

# Or
../build/build-static-13/tests/tests
```

## Licence, GPL v3.0
Copyright 2021, Luc Hermitte

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see http://www.gnu.org/licenses/.
