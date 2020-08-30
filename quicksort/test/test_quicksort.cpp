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

void assert_is_finished_with_pivot(std::variant<basic_partition_step, size_t> result, size_t pivot) {
    CHECK(std::holds_alternative<size_t>(result));
    CHECK(std::get<size_t>(result) == pivot);
}

void assert_is_unfinished_with_step(std::variant<basic_partition_step, size_t> result, std::pair<size_t, size_t> step) {
    CHECK(std::holds_alternative<basic_partition_step>(result));
    CHECK(std::get<basic_partition_step>(result).le_end == step.first);
    CHECK(std::get<basic_partition_step>(result).ge_begin == step.second);
}

TEST_CASE( "basic partition advances correctly", "[basic_partition]" ) {
    std::vector<int> array = { 40, 40, 60, 10, 50, 10, 70 };
    auto step = basic_partition_step::initial(vector_view{&array});

    auto result = step.step();

    assert_is_unfinished_with_step(result, { 1, 6 });
}

TEST_CASE( "basic partition terminates correctly", "[basic_partition]" ) {
    std::vector<int> array = { 40, 40, 10, 10, 50, 60, 70 };
    auto step = basic_partition_step::initial(vector_view{&array}).set_state(1, 6);

    auto result = step.step();

    assert_is_finished_with_pivot(result, 3);
}

TEST_CASE( "basic partition terminates correctly on all equal cases", "[basic_partition]" ) {
    std::vector<int> array = { 10, 10, 10 };
    auto step = basic_partition_step::initial(vector_view{&array});

    auto result = step.step();

    assert_is_finished_with_pivot(result, 2);
}

TEST_CASE( "basic partition terminates correctly on le edge cases", "[basic_partition]" ) {
    std::vector<int> array = { 10, 10, 20 };
    auto step = basic_partition_step::initial(vector_view{&array});

    auto result = step.step();

    assert_is_finished_with_pivot(result, 1);
}


TEST_CASE( "basic partition terminates correctly on ge edge cases", "[basic_partition]" ) {
    std::vector<int> array = { 10, 20, 20 };
    auto step = basic_partition_step::initial(vector_view{&array});

    auto result = step.step();

    assert_is_finished_with_pivot(result, 0);
}