# Copyright (c) 2025 Maxtek Consulting
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.15)

project(hyperpage)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(HYPERPAGE_TESTS "Build tests" OFF)
option(HYPERPAGE_COVER "Enable code coverage" OFF)
option(HYPERPAGE_EXAMPLE "Build example" OFF)
option(HYPERPAGE_DOCS "Build documentation" OFF)

include(FetchContent)
FetchContent_Declare(
    MegaMimes
    GIT_REPOSITORY https://github.com/kobbyowen/MegaMimes.git
    GIT_TAG master
)
FetchContent_MakeAvailable(MegaMimes)

FetchContent_Declare(
    sqlite3
    GIT_REPOSITORY https://github.com/sjinks/sqlite3-cmake.git
    GIT_TAG master
)
FetchContent_MakeAvailable(sqlite3)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG master
)
FetchContent_MakeAvailable(argparse)
FetchContent_Declare(
    mio
    GIT_REPOSITORY https://github.com/maxtek6/mio.git
    GIT_TAG master
)
FetchContent_MakeAvailable(mio)

add_library(
    hyperpage 
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/hyperpage.cpp
    ${megamimes_SOURCE_DIR}/src/MegaMimes.c
)

target_include_directories(hyperpage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} 
    ${megamimes_SOURCE_DIR}/src)
target_link_libraries(hyperpage PUBLIC SQLite::SQLite3)

add_executable(hyperpack ${CMAKE_CURRENT_SOURCE_DIR}/hyperpack.cpp)
target_include_directories(hyperpack PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hyperpack PRIVATE argparse hyperpage mio::mio)

if(HYPERPAGE_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

if(HYPERPAGE_EXAMPLE)
    add_subdirectory(example)
endif()

if(HYPERPAGE_DOCS)
    add_subdirectory(docs)
endif()