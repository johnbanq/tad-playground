cmake_minimum_required(VERSION 3.10)

project(
  tgi-quicksort-test
  VERSION 1.0
  LANGUAGES CXX
)


add_executable(
    sanity-check
    test_sanity.cpp
)
target_link_libraries(sanity-check PRIVATE Catch2::Catch)
add_test(Test::Sanity sanity-check)