#include <vector>

#include "catch.hpp"

#include "tgi/substr/bruteforce.h"

// bruteforce test //

TEST_CASE( "substr_bruteforce works", "[bruteforce]" ) {
    std::string_view source{"abcde"};
    std::string_view pattern{"cde"};
    CHECK(substr_bruteforce(source, pattern) == 2);
}

TEST_CASE( "substr_bruteforce rejects not found string", "[bruteforce]" ) {
    std::string_view source{"abcde"};
    std::string_view pattern{"xyz"};
    CHECK(substr_bruteforce(source, pattern) == -1);
}

TEST_CASE( "substr_bruteforce handles extreme cases", "[bruteforce]" ) {
    CHECK(substr_bruteforce("abc", "") == 0);
    CHECK(substr_bruteforce("", "abc") == -1);
    CHECK(substr_bruteforce("x", "x") == 0);
}

TEST_CASE( "pattern_match_from_begin works", "[bruteforce]" ) {
    CHECK(pattern_match_from_begin("abcde", "abc"));
}

TEST_CASE( "pattern_match_from_begin handles shorter source correctly", "[bruteforce]" ) {
    CHECK_FALSE(pattern_match_from_begin("ab", "abc"));
}
