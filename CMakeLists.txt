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
    GIT_TAG b839068db99cbfcff1af8df1229bd7e41701fe96
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

target_include_directories(
    hyperpage 
    PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${megamimes_SOURCE_DIR}/src)

target_link_libraries(hyperpage PUBLIC SQLite::SQLite3)

set_target_properties(hyperpage PROPERTIES
    PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/hyperpage.hpp"
)

add_executable(hyperpack ${CMAKE_CURRENT_SOURCE_DIR}/hyperpack.cpp)
target_include_directories(hyperpack PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hyperpack PRIVATE argparse hyperpage mio::mio)


# CMake function for creating hyperpack archives
#
# Usage: hyperpage_add_archive(<name> <directory>)
#
# This function creates a CMake target that will run hyperpack to create
# an archive database from the specified directory during the build process.
#
# Parameters:
#   name      - Name of the target and output database file (without .db extension)
#   directory - Directory to pack into the hyperpage database
#
# The function will:
#   - Create a custom target named <name>
#   - Generate <name>.db in CMAKE_CURRENT_BINARY_DIR
#   - Add dependency on the hyperpack executable
#   - Set HYPERPAGE_ARCHIVE_FILE property on the target for the output file path
#
# Example:
#   hyperpage_add_archive(my_content "${CMAKE_CURRENT_SOURCE_DIR}/web_assets")
#   add_dependencies(my_server my_content)  # Ensure archive is built before server
#
function(hyperpage_add_archive name directory)
    # Validate arguments
    if(NOT name)
        message(FATAL_ERROR "hyperpage_add_archive: Archive name must be specified")
    endif()
    
    if(NOT directory)
        message(FATAL_ERROR "hyperpage_add_archive: Directory must be specified")
    endif()
    
    # Convert to absolute path for better reliability
    get_filename_component(abs_directory "${directory}" ABSOLUTE)
    
    # Create the output database file name
    set(output_file "${CMAKE_CURRENT_BINARY_DIR}/${name}.db")
    
    # Create a custom target for this archive
    add_custom_target(${name}
        DEPENDS ${output_file}
        COMMENT "Building hyperpack archive: ${name}"
    )
    
    # Add dependency on hyperpack executable
    add_dependencies(${name} hyperpack)
    
    # Create custom command to generate the archive
    add_custom_command(
        OUTPUT ${output_file}
        COMMAND $<TARGET_FILE:hyperpack> -o ${output_file} ${abs_directory}
        DEPENDS hyperpack
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Creating hyperpack archive ${name} from directory: ${abs_directory}"
        VERBATIM
    )
    
    # Set a property so consumers can find the generated file
    set_target_properties(${name} PROPERTIES
        HYPERPAGE_ARCHIVE_FILE "${output_file}"
    )
endfunction()

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

install (TARGETS hyperpage hyperpack
    EXPORT hyperpage-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT hyperpage-targets
    FILE hyperpage-targets.cmake
    NAMESPACE hyperpage::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hyperpage
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/hyperpage-config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hyperpage
)