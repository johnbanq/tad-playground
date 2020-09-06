cmake_minimum_required(VERSION 3.10)

project(
  tgi-substr
  VERSION 1.0
  LANGUAGES CXX
)


# dependencies #
add_subdirectory(lib/catch)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(Catch)

# target definitions #

add_library(
    substr 
        src/bruteforce.cpp
)
target_include_directories(substr PUBLIC include)
target_compile_features(substr PUBLIC cxx_std_17)

# enable tests #

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    add_subdirectory(test)
endif()