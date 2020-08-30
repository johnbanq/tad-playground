#include <vector>

#include "catch.hpp"

#include "tgi/quicksort/quicksort.h"

TEST_CASE( "quicksort works on odd  len array", "[quicksort]" ) {
    REQUIRE(quicksort(std::vector<int>{30, 29, 17}) == std::vector<int>{17, 29, 30});
}

TEST_CASE( "quicksort works on even length array", "[quicksort]" ) {
    REQUIRE(quicksort(std::vector<int>{87, 31}) == std::vector<int>{31, 87});
}