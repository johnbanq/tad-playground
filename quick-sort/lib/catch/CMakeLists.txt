cmake_minimum_required(VERSION 3.10)


add_library(
    Catch2
        STATIC 
            src/catch.cpp
)
add_library(Catch2::Catch ALIAS Catch2)
target_include_directories(Catch2 PUBLIC include)