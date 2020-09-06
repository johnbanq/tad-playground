#include <vector>

#include "catch.hpp"

#include "tgi/substr/bruteforce.h"

// bruteforce test //

TEST_CASE( "substr_bruteforce works", "[bruteforce]" ) {
    CHECK(substr_bruteforce("abcde", "cde") == 2);
}

TEST_CASE( "substr_bruteforce rejects not found string", "[bruteforce]" ) {
    CHECK(substr_bruteforce("abcde", "xyz") == -1);
}

TEST_CASE( "substr_bruteforce handles extreme cases", "[bruteforce]" ) {
    CHECK(substr_bruteforce("abc", "") == 0);
    CHECK(substr_bruteforce("", "abc") == -1);
    CHECK(substr_bruteforce("x", "x") == 0);
}