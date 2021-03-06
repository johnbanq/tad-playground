cmake_minimum_required(VERSION 3.10)

project(
  tgi-bst
  VERSION 1.0
  LANGUAGES CXX
)


# dependencies #
add_subdirectory(lib/catch)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(Catch)

# target definitions #

add_library(
    bst
        src/naive_bst.cpp
        src/avl.cpp
        src/rbtree.cpp
)
target_include_directories(bst PUBLIC include)
target_compile_features(bst PUBLIC cxx_std_17)

# enable tests #

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    add_subdirectory(test)
endif()