#include <vector>

#include "catch.hpp"

#include "tgi/substr/rabinkarp.h"

// rabinkarp test //

TEST_CASE( "substr_rabinkarp works", "[rabinkarp]" ) {
    CHECK(substr_rabinkarp("abcde", "cde") == 2);
}

TEST_CASE( "substr_rabinkarp rejects not found string", "[rabinkarp]" ) {
    CHECK(substr_rabinkarp("abcde", "xyz") == -1);
}

TEST_CASE( "substr_rabinkarp handles extreme cases", "[rabinkarp]" ) {
    CHECK(substr_rabinkarp("abc", "") == 0);
    CHECK(substr_rabinkarp("", "abc") == -1);
    CHECK(substr_rabinkarp("x", "x") == 0);
}

TEST_CASE( "hash_range works", "[rabinkarp]" ) {
    //python:
    //    ord('a')*((2**8)**2)+ord('b')*((2**8)**1)+ord('c')*((2**8)**0) == 6382179
    //    6382179 % 997 == 382
    CHECK(hash_range("abc") == 382);
}


TEST_CASE( "hash_slidewindow_iterator works", "[rabinkarp]" ) {
    std::string_view source("abcd");
    hash_slidewindow_iterator iter { source, 3};
    
    CHECK(iter.has_next_window());
    CHECK(iter.hash_value() == hash_range("abc"));
    CHECK(std::string(iter.window()) == "abc");

    iter.try_slide_right();

    CHECK(!iter.has_next_window());
    CHECK(iter.hash_value() == hash_range("bcd"));
    CHECK(std::string(iter.window()) == "bcd");
}