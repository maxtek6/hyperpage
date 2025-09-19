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

FetchContent_Declare(
    Maxtest
    GIT_REPOSITORY https://github.com/maxtek6/maxtest.git
    GIT_TAG master
)
FetchContent_MakeAvailable(Maxtest)

maxtest_add_executable(
    unit ${CMAKE_CURRENT_SOURCE_DIR}/unit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../hyperpage.cpp
    ${megamimes_SOURCE_DIR}/src/MegaMimes.c)

target_include_directories(unit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ ${megamimes_SOURCE_DIR}/src ${SQLITE3_INCLUDE_DIRS})

if(HYPERPAGE_COVER)
    if(WIN32)
        message("skipping code coverage for windows")
        target_link_libraries(unit PRIVATE ${SQLITE3_LIBRARIES})
    else()
        target_compile_options(unit PRIVATE -fprofile-arcs -ftest-coverage -g -O0)
        target_link_libraries(unit PRIVATE ${SQLITE3_LIBRARIES} gcov "--coverage")        
        add_custom_target(
            cover
            DEPENDS unit)
        add_custom_command(
            TARGET cover POST_BUILD
            COMMAND gcovr -r ${CMAKE_CURRENT_SOURCE_DIR}/.. -e ${CMAKE_CURRENT_SOURCE_DIR})
        endif()
else()
    target_link_libraries(unit PRIVATE ${SQLITE3_LIBRARIES})
endif()

maxtest_add_test(unit store_load $<TARGET_FILE_DIR:unit>)
maxtest_add_test(unit open_database $<TARGET_FILE_DIR:unit>)
maxtest_add_test(unit mime_type $<TARGET_FILE_DIR:unit>)
maxtest_add_test(unit overwrite_test $<TARGET_FILE_DIR:unit>)
maxtest_add_test(unit archive_size_no_growth_test $<TARGET_FILE_DIR:unit>)