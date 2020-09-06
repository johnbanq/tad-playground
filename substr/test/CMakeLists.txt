cmake_minimum_required(VERSION 3.10)

project(
  tgi-substr-test
  VERSION 1.0
  LANGUAGES CXX
)

add_executable(
    sanity_test
    test_sanity.cpp
)
target_link_libraries(sanity_test PRIVATE Catch2::Catch substr)
catch_discover_tests(sanity_test)

add_executable(
    bruteforce_test
    test_bruteforce.cpp
)
target_link_libraries(bruteforce_test PRIVATE Catch2::Catch substr)
catch_discover_tests(bruteforce_test)

add_executable(
    rabinkarp_test
    test_rabinkarp.cpp
)
target_link_libraries(rabinkarp_test PRIVATE Catch2::Catch substr)
catch_discover_tests(rabinkarp_test)