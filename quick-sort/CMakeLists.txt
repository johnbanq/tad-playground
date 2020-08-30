cmake_minimum_required(VERSION 3.10)

project(
  tgi-quicksort
  VERSION 1.0
  LANGUAGES CXX
)


# dependencies #
add_subdirectory(lib/catch)

# target definitions #

add_library(
    quicksort 
        src/stub.cpp
)
target_link_libraries(quicksort PUBLIC include)
target_compile_features(quicksort PUBLIC cxx_std_17)

# enable tests #

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    add_subdirectory(test)
endif()