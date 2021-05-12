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

cmake_minimum_required(VERSION 3.19)
project(tests LANGUAGES NONE)

enable_testing()
include(CTest)

if (CMAKE_HOST_WIN32)
    set(static_opts -DCMAKE_RELEASE_POSTFIX=_static)
    set(LDD dumpbin /dependents)
    set(EXT .exe)
elseif (CMAKE_HOST_APPLE)
    set(LDD otool -L)
else ()
    set(LDD ldd)
endif ()

# ======================================================================
## DOCTEST
include(ExternalProject)
find_package(Git REQUIRED)

option(UPDATE_DOCTEST "Request to update doctest dependency" OFF)
if (UPDATE_DOCTEST)
  set(UPDATE_DISCONNECTED_DOCTEST 0)
else()
  set(UPDATE_DISCONNECTED_DOCTEST 1)
endif()

include(FetchContent)
FetchContent_Declare(
    doctest
    PREFIX ${CMAKE_BINARY_DIR}/doctest
    GIT_REPOSITORY https://github.com/onqtam/doctest.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    UPDATE_DISCONNECTED "${UPDATE_DISCONNECTED_DOCTEST}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)
FetchContent_MakeAvailable(doctest)
# Expose required variable (doctest_INCLUDE_DIR) to parent scope
# ExternalProject_Get_Property(doctest source_dir)
# set(doctest_INCLUDE_DIR ${source_dir}/doctest CACHE INTERNAL "Path to include folder for doctest")
set(doctest_INCLUDE_DIR ${doctest_SOURCE_DIR}/doctest CACHE INTERNAL "Path to include folder for doctest")
# set(doctest_SOURCE_DIR ${source_dir} CACHE INTERNAL "Path to include folder for doctest")

add_library(Doctest INTERFACE)
add_dependencies(Doctest doctest)
target_include_directories(Doctest INTERFACE ${doctest_INCLUDE_DIR})

message(STATUS "doctest_INCLUDE_DIR: ${doctest_INCLUDE_DIR}")
message(STATUS "doctest_SOURCE_DIR: ${doctest_SOURCE_DIR}")

# ======================================================================
# Set up install trees for subsequent tests
### add it globally
##include_directories(${doctest_INCLUDE_DIR})
##
### or per target
add_executable(tests
    main-4tests.cpp
    test_bitset.cpp
    test_grid.cpp
    )
add_dependencies(tests Doctest)
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

# target_link_libraries(tests INTERFACE doctest)
target_link_libraries(tests PUBLIC SudokuLib::SudokuLib)
# SudokuLib already officially depends on doctest, no need to be explicit
target_include_directories(tests PRIVATE "${doctest_INCLUDE_DIR}")

doctest_discover_tests(tests
    ADD_LABELS 1)

# Then (cd build-shared/tests && ctest)
