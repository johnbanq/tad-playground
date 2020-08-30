cmake_minimum_required(VERSION 3.10)

project(
  tgi-quicksort-test
  VERSION 1.0
  LANGUAGES CXX
)

add_executable(
    quicksort_test
    test_sanity.cpp
    test_quicksort.cpp
)
target_link_libraries(quicksort_test PRIVATE Catch2::Catch quicksort)
catch_discover_tests(quicksort_test)