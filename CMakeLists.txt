#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or(at
#  your option) any later version.
#
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http: //www.gnu.org/licenses/>.

CMAKE_MINIMUM_REQUIRED(VERSION 3.19)

project(Sudoku VERSION 1.0.0
    DESCRIPTION "POC: Soduku and C++20"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# set(CMAKE_CXX_VISIBILITY_PRESET hidden)
# set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# ======================================================================
## Dependencies
#
include(FetchContent)

# ====< cppcoro
option(UPDATE_CPPCORO "Request to update cppcoro dependency" OFF)
if (UPDATE_CPPCORO)
  set(UPDATE_DISCONNECTED_CPPCORO 0)
else()
  set(UPDATE_DISCONNECTED_CPPCORO 1)
endif()

FetchContent_Declare(
    cppcoro
    PREFIX ${CMAKE_BINARY_DIR}/cppcoro
    GIT_REPOSITORY https://github.com/lewissbaker/cppcoro
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    UPDATE_DISCONNECTED "${UPDATE_DISCONNECTED_CPPCORO}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)
FetchContent_MakeAvailable(cppcoro)
set(cppcoro_INCLUDE_DIR ${cppcoro_SOURCE_DIR}/include CACHE INTERNAL "Path to include folder for cppcoro")

add_library(Cppcoro INTERFACE)
add_dependencies(Cppcoro cppcoro)
target_include_directories(Cppcoro INTERFACE ${cppcoro_INCLUDE_DIR})
message(STATUS "cppcoro_INCLUDE_DIR: ${cppcoro_INCLUDE_DIR}")

# ====< mdspan
option(UPDATE_MDSPAN "Request to update mdspan dependency" OFF)
if (UPDATE_MDSPAN)
  set(UPDATE_DISCONNECTED_MDSPAN 0)
else()
  set(UPDATE_DISCONNECTED_MDSPAN 1)
endif()

FetchContent_Declare(
    mdspan
    PREFIX ${CMAKE_BINARY_DIR}/mdspan
    GIT_REPOSITORY https://github.com/kokkos/mdspan
    GIT_TAG stable
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    UPDATE_DISCONNECTED "${UPDATE_DISCONNECTED_MDSPAN}"
    # CONFIGURE_COMMAND ""
    # BUILD_COMMAND ""
    # INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    PATCH_COMMAND git apply --ignore-space-change --ignore-whitespace ${CMAKE_CURRENT_SOURCE_DIR}/cmake/mdspan-unused-param.patch
)
FetchContent_MakeAvailable(mdspan)
# list(APPEND CMAKE_MODULE_PATH ${mdspan_BINARY_DIR})
set(MDSpan_DIR ${mdspan_BINARY_DIR})
set(mdspan_INCLUDE_DIR ${mdspan_SOURCE_DIR}/include CACHE INTERNAL "Path to include folder for mdspan")

add_library(Mdspan INTERFACE)
add_dependencies(Mdspan mdspan)
target_include_directories(Mdspan INTERFACE ${mdspan_INCLUDE_DIR})
message(STATUS "mdspan_INCLUDE_DIR: ${mdspan_INCLUDE_DIR}")
message(STATUS "mdspan_BINARY_DIR: ${mdspan_BINARY_DIR}")
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

# ====< mdarray
option(UPDATE_MDARRAY "Request to update mdarray dependency" OFF)
if (UPDATE_MDARRAY)
  set(UPDATE_DISCONNECTED_MDARRAY 0)
else()
  set(UPDATE_DISCONNECTED_MDARRAY 1)
endif()

FetchContent_Declare(
    mdarray
    PREFIX ${CMAKE_BINARY_DIR}/mdarray
    GIT_REPOSITORY https://github.com/kokkos/mdarray
    # GIT_TAG stable
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    UPDATE_DISCONNECTED "${UPDATE_DISCONNECTED_MDARRAY}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    PATCH_COMMAND git apply --ignore-space-change --ignore-whitespace
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/mdarray-cmakelists.patch
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/mdarray-fix-offset.patch
)
FetchContent_MakeAvailable(mdarray)
set(mdarray_INCLUDE_DIR ${mdarray_SOURCE_DIR}/include CACHE INTERNAL "Path to include folder for mdarray")

add_library(Mdarray INTERFACE)
add_dependencies(Mdarray mdarray)
target_include_directories(Mdarray INTERFACE ${mdarray_INCLUDE_DIR})
message(STATUS "mdarray_INCLUDE_DIR: ${mdarray_INCLUDE_DIR}")
# TODO: pass mdspan to mdarray

# ======================================================================
if (DEFINED SudokuLib_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${SudokuLib_SHARED_LIBS}")
endif ()
add_library(SudokuLib)
add_library(SudokuLib::SudokuLib ALIAS SudokuLib)
set_target_properties(SudokuLib PROPERTIES
    VERSION ${SudokuLib_VERSION}
    SOVERSION ${SudokuLib_VERSION_MAJOR})
add_dependencies(SudokuLib Cppcoro) # Make sure to fetch cppcoro before anything else
add_dependencies(SudokuLib Mdspan) # Make sure to fetch mdspan before anything else
add_dependencies(SudokuLib Mdarray) # Make sure to fetch mdarray before anything else
target_compile_features(SudokuLib PUBLIC cxx_std_20)
target_include_directories(SudokuLib PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
# Use BUILD_INTERFACE as doctest is used within sources
target_include_directories(SudokuLib PUBLIC "$<BUILD_INTERFACE:${cppcoro_INCLUDE_DIR}>")
target_include_directories(SudokuLib PUBLIC "$<BUILD_INTERFACE:${mdspan_INCLUDE_DIR}>")
target_include_directories(SudokuLib PUBLIC "$<BUILD_INTERFACE:${mdarray_INCLUDE_DIR}>")
target_sources(SudokuLib
    PRIVATE   src/solver.cpp
    PRIVATE   src/grid.cpp
    # PUBLIC    include/sudokus.h
    PUBLIC     "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/sudoku/bitset.hpp>"
               "$<INSTALL_INTERFACE:include/sudoku/bitset.hpp>"
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/sudoku/cell.hpp>"
               "$<INSTALL_INTERFACE:include/sudoku/cell.hpp>"
    )

## Generate the export header for SudokuLib and attach it to the target
include(GenerateExportHeader)
generate_export_header(SudokuLib EXPORT_FILE_NAME include/sudokulib/export.h)
target_compile_definitions(SudokuLib PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:SUDOKULIB_STATIC_DEFINE>")
target_include_directories(SudokuLib PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

add_executable(sudoku app/main.cpp)
target_link_libraries(sudoku PUBLIC SudokuLib::SudokuLib)

## Include the install rules if the user wanted them (included by default when top-level)
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)
option(SudokuLib_INCLUDE_PACKAGING "Include packaging rules for SudokuLib" "${is_top_level}")
if (SudokuLib_INCLUDE_PACKAGING)
    add_subdirectory(packaging)
endif ()


# ======================================================================
## Tests
option(BUILD_TESTING "Enable tests" ON)
if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
