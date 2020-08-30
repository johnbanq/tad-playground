#include <vector>

#include "catch.hpp"

#include "tgi/quicksort/quicksort.h"

TEST_CASE( "quicksort works on odd len array", "[quicksort]" ) {
    REQUIRE(quicksort(std::vector<int>{30, 29, 17}) == std::vector<int>{17, 29, 30});
}

TEST_CASE( "quicksort works on even length array", "[quicksort]" ) {
    REQUIRE(quicksort(std::vector<int>{87, 31}) == std::vector<int>{31, 87});
}

// basic partition

void assert_is_finished_with_pivot(basic_partition_step_result result, size_t pivot) {
    REQUIRE(result.finished);
    REQUIRE(result.data.pivot == pivot);
}

void assert_is_unfinished_with_step(basic_partition_step_result result, basic_partition_step step) {
    REQUIRE(!result.finished);
    REQUIRE(result.data.step.le_end == step.le_end);
    REQUIRE(result.data.step.ge_begin == step.ge_begin);
}

TEST_CASE( "basic partition advances correctly", "[basic_partition]" ) {
    std::vector<int> array = { 40, 40, 60, 10, 50, 10, 70 };
    basic_partition_step initial_step { 0, array.size() };

    auto result = basic_quicksort_partition_step({array, 0, array.size()}, initial_step);

    assert_is_unfinished_with_step(result, { 1, 6 });
}

TEST_CASE( "basic partition terminates correctly", "[basic_partition]" ) {
    std::vector<int> array = { 40, 40, 10, 10, 50, 60, 70 };
    basic_partition_step initial_step { 1, 6 };

    auto result = basic_quicksort_partition_step({array, 0, array.size()}, initial_step);

    assert_is_finished_with_pivot(result, 3);
}

TEST_CASE( "basic partition terminates correctly on all equal cases", "[basic_partition]" ) {
    std::vector<int> array = { 10, 10, 10 };
    basic_partition_step initial_step { 0, array.size() };

    auto result = basic_quicksort_partition_step({array, 0, array.size()}, initial_step);

    assert_is_finished_with_pivot(result, 2);
}

TEST_CASE( "basic partition terminates correctly on le edge cases", "[basic_partition]" ) {
    std::vector<int> array = { 10, 10, 20 };
    basic_partition_step initial_step { 0, array.size() };

    auto result = basic_quicksort_partition_step({array, 0, array.size()}, initial_step);

    assert_is_finished_with_pivot(result, 1);
}


TEST_CASE( "basic partition terminates correctly on ge edge cases", "[basic_partition]" ) {
    std::vector<int> array = { 10, 20, 20 };
    basic_partition_step initial_step { 0, array.size() };

    auto result = basic_quicksort_partition_step({array, 0, array.size()}, initial_step);

    assert_is_finished_with_pivot(result, 0);
}

basic_partition_step_result basic_quicksort_partition_step(vector_view v, basic_partition_step step);