cmake_minimum_required(VERSION 3.8)
project(StrideParser_tests)

option(BUILD_STRIDEPARSER_TESTS "Build and run unit test for the Stride parser" OFF)

# Get and build googletest
if (NOT gtest)
    message("Building googletest with strideparser build")
    include(FetchContent)
    FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.10.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()


set(TEST_SOURCES main.cpp
  basic.cpp
  schema.cpp
  library.cpp
)


add_executable(strideparser_tests ${TEST_SOURCES})

set_target_properties(strideparser_tests PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

add_test(NAME strideparser_tests COMMAND strideparser_tests)

target_link_libraries(strideparser_tests PUBLIC StrideParser gtest)
target_compile_definitions(strideparser_tests PUBLIC
  "-DTESTS_SOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data/\"")

if (STRIDEPARSER_CODE_COVERAGE)
  set_target_properties(strideparser_tests PROPERTIES COMPILE_FLAGS "-fPIE -g -O0 -Wall -fprofile-arcs -ftest-coverage")
  target_link_libraries(strideparser_tests PUBLIC gcov)
endif()
