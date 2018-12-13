# ============================================================================
#                  SeqAn - The Library for Sequence Analysis
# ============================================================================
#
# Copyright (c) 2006-2018, Knut Reinert & Freie Universitaet Berlin
# Copyright (c) 2016-2018, Knut Reinert & MPI Molekulare Genetik
# Copyright (c) 2012-2017, Lars Bilke (Adopted from CodeCoverage.cmake; BSD-License)
#     https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Knut Reinert or the FU Berlin nor the names of
#       its contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL KNUT REINERT OR THE FU BERLIN BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
# DAMAGE.
# ============================================================================

cmake_minimum_required (VERSION 3.2)
project (seqan3_test_coverage CXX)

include (../seqan3-test.cmake)

if (CMAKE_BUILD_TYPE AND NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
    message(WARNING "Coverage test must be build in debug mode [build type = ${CMAKE_BUILD_TYPE}]")
endif ()

find_program (LCOV_COMMAND  NAMES lcov lcov.bat lcov.exe lcov.perl)
find_program (GENHTML_COMMAND NAMES genhtml genhtml.perl genhtml.bat)

if (NOT LCOV_COMMAND)
    message(FATAL_ERROR "lcov not found! Aborting...")
endif ()

# Files which should not be included in the coverage report
set (TEST_COVERAGE_EXCLUDE_FILES
    "'/usr/*'"
    "'${SEQAN3_CLONE_DIR}/submodules/*'"
    "'${SEQAN3_CLONE_DIR}/test/unit/*'"
    "'${PROJECT_BINARY_DIR}/vendor/*'"
)
# Holds all target's defined by seqan3_test
set_property (GLOBAL PROPERTY GLOBAL_TEST_COVERAGE_ALL_TESTS "")

add_custom_command (
    OUTPUT ${PROJECT_BINARY_DIR}/seqan3_coverage
    # Cleanup lcov (resetting code coverage counters to zero)
    COMMAND ${LCOV_COMMAND} --directory ${PROJECT_BINARY_DIR} --zerocounters
    # Create baseline to make sure untouched files show up in the report
    COMMAND ${LCOV_COMMAND} --directory ${PROJECT_BINARY_DIR} --capture --initial --output-file seqan3_coverage.baseline

    # Run tests
    COMMAND ${CMAKE_CTEST_COMMAND}

    # Capturing lcov counters and generating report
    COMMAND ${LCOV_COMMAND} --directory ${PROJECT_BINARY_DIR} --capture --output-file seqan3_coverage.captured
    # merge baseline counters and captured counters
    COMMAND ${LCOV_COMMAND} -a seqan3_coverage.baseline -a seqan3_coverage.captured --output-file seqan3_coverage.total
    COMMAND ${LCOV_COMMAND} --remove seqan3_coverage.total ${TEST_COVERAGE_EXCLUDE_FILES} --output-file ${PROJECT_BINARY_DIR}/seqan3_coverage

    BYPRODUCTS seqan3_coverage.baseline seqan3_coverage.captured seqan3_coverage.total

    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Processing code coverage counters."
)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "seqan3_coverage.baseline;seqan3_coverage.captured;seqan3_coverage.total")

add_custom_target (
    coverage ALL
    DEPENDS
    ${PROJECT_BINARY_DIR}/seqan3_coverage
)

add_custom_target (
    coverage_html
    COMMAND ${GENHTML_COMMAND} --highlight --legend --output-directory html ${PROJECT_BINARY_DIR}/seqan3_coverage
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS coverage
    COMMENT "Generate coverage report."
)

add_custom_command(
    TARGET coverage_html POST_BUILD
    COMMAND ;
    COMMENT "Open ${PROJECT_BINARY_DIR}/html/index.html in your browser to view the coverage report."
)

# overload seqan3_test to fit
macro (seqan3_test target_cpp)
    # $target_cpp = "pod_tuple_test.cpp"
    #   * will register the global TARGET name "pod_tuple_test" and
    #   * will register the test case name "core_pod_tuple" if
    #     pod_tuple_test.cpp is in test/core/
    #
    # NOTE(marehr): ".+/test/" REGEX is greedy, that means
    # /test/test/test/hello_test.cpp will result in an empty `test_path`
    string (REGEX REPLACE "_test.cpp$" "" target_name ${target_cpp})
    string (REGEX REPLACE ".+/test/" "" test_path ${CMAKE_CURRENT_LIST_DIR})
    set (target "${target_name}_coverage_test")

    add_executable (${target} ${target_cpp})
    target_link_libraries (${target} seqan3::test::coverage)
    add_test (NAME "${test_path}/${target}" COMMAND ${target})

    # any change of a target will invalidate the coverage result;
    # NOTE that this is a GLOBAL variable, because a normal
    # `set(GLOBAL_TEST_COVERAGE_ALL_TESTS)` would not propagate the result when
    # CMakeLists.txt goes out of scope due to a `add_subdirectory`
    set_property(GLOBAL APPEND PROPERTY GLOBAL_TEST_COVERAGE_ALL_TESTS ${target})

    unset (target_name)
    unset (test_path)
    unset (target)
endmacro ()

seqan3_require_ccache ()
seqan3_require_test ()

# add all unit tests
add_subdirectories_of ("${CMAKE_CURRENT_SOURCE_DIR}/../unit")

# add collected test cases as dependency
get_property(TEST_COVERAGE_ALL_TESTS GLOBAL PROPERTY GLOBAL_TEST_COVERAGE_ALL_TESTS)
add_custom_command (
    OUTPUT ${PROJECT_BINARY_DIR}/seqan3_coverage
    DEPENDS ${TEST_COVERAGE_ALL_TESTS}
    APPEND
)