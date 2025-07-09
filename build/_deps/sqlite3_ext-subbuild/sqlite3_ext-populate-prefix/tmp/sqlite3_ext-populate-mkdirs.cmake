# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/runner/work/hyperpage/hyperpage/build/_deps/sqlite3_ext-src")
  file(MAKE_DIRECTORY "/home/runner/work/hyperpage/hyperpage/build/_deps/sqlite3_ext-src")
endif()
file(MAKE_DIRECTORY
  "/home/runner/work/hyperpage/hyperpage/build/_deps/sqlite3_ext-build"
  "/home/runner/work/hyperpage/hyperpage/build/_deps/sqlite3_ext-subbuild/sqlite3_ext-populate-prefix"
  "/home/runner/work/hyperpage/hyperpage/build/_deps/sqlite3_ext-subbuild/sqlite3_ext-populate-prefix/tmp"
  "/home/runner/work/hyperpage/hyperpage/build/_deps/sqlite3_ext-subbuild/sqlite3_ext-populate-prefix/src/sqlite3_ext-populate-stamp"
  "/home/runner/work/hyperpage/hyperpage/build/_deps/sqlite3_ext-subbuild/sqlite3_ext-populate-prefix/src"
  "/home/runner/work/hyperpage/hyperpage/build/_deps/sqlite3_ext-subbuild/sqlite3_ext-populate-prefix/src/sqlite3_ext-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/runner/work/hyperpage/hyperpage/build/_deps/sqlite3_ext-subbuild/sqlite3_ext-populate-prefix/src/sqlite3_ext-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/runner/work/hyperpage/hyperpage/build/_deps/sqlite3_ext-subbuild/sqlite3_ext-populate-prefix/src/sqlite3_ext-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
