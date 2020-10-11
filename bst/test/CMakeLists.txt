cmake_minimum_required(VERSION 3.10)

project(
  tgi-bst-test
  VERSION 1.0
  LANGUAGES CXX
)

add_executable(
    sanity_test
    test_sanity.cpp
)
target_link_libraries(sanity_test PRIVATE Catch2::Catch bst)
catch_discover_tests(sanity_test)

add_executable(
    naive_bst_test
    test_naive_bst.cpp
)
target_link_libraries(naive_bst_test PRIVATE Catch2::Catch bst)
catch_discover_tests(naive_bst_test)

add_executable(
    avl_test
    test_avl.cpp
)
target_link_libraries(avl_test PRIVATE Catch2::Catch bst)
catch_discover_tests(avl_test)