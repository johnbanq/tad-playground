#include <vector>

#include "catch.hpp"

#include "tgi/quicksort/quicksort.h"

TEST_CASE( "quicksort works on 3", "[quicksort]" ) {
    REQUIRE(quicksort(std::vector<int>{3,2,1}) == std::vector<int>{1, 2, 3});
}