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

add_executable(
    avl_tampering_test
    avl_tampering.cpp
)
target_link_libraries(avl_tampering_test PRIVATE Catch2::Catch bst)
catch_discover_tests(avl_tampering_test)

add_executable(
    rbtree_test
    test_rbtree.cpp
)
target_link_libraries(rbtree_test PRIVATE Catch2::Catch bst)
catch_discover_tests(rbtree_test)

add_executable(
    rbtree_tampering_test
    rbtree_tampering.cpp
)
target_link_libraries(rbtree_tampering_test PRIVATE Catch2::Catch bst)
catch_discover_tests(rbtree_tampering_test)