#include <vector>

#include "catch.hpp"

#include "tgi/quicksort/quicksort.h"

// general test //

TEST_CASE( "quicksort works", "[quicksort]" ) {
    std::vector<std::unique_ptr<partition_algorithm>> algorithms;
    algorithms.emplace_back(new fake_partition_algorithm());
    algorithms.emplace_back(new basic_partition_algorithm());
    algorithms.emplace_back(new knuth_partition_algorithm());

    for(auto& algorithm: algorithms) {
        REQUIRE(quicksort(std::vector<int>{}, algorithm.get()) == std::vector<int>{});
        REQUIRE(quicksort(std::vector<int>{13}, algorithm.get()) == std::vector<int>{13});
        REQUIRE(quicksort(std::vector<int>{30, 29, 17}, algorithm.get()) == std::vector<int>{17, 29, 30});
        REQUIRE(quicksort(std::vector<int>{87, 31}, algorithm.get()) == std::vector<int>{31, 87});
    }
}

// basic partition //

using basic_partition_step = basic_partition_algorithm::basic_partition_step;

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

// knuth partition //

using three_way_struct = knuth_partition_algorithm::three_way_struct;

std::pair<int, three_way_struct> generate_testcase(size_t less_size, size_t equal_size, size_t greater_size, std::vector<int>& v) {
    int value = 10;
    for (size_t i = 0; i < less_size; i++) {
        v.push_back(value);
        value += 10;
    }
    int pivot = value;
    for (size_t i = 0; i < equal_size; i++) {
        v.push_back(value);
    }
    for (size_t i = 0; i < less_size; i++) {
        value += 10;
        v.push_back(value);
    }
    return std::make_pair(
        pivot, 
        three_way_struct::initial(vector_view{&v})
            .set_state(
                less_size, 
                less_size + equal_size,
                less_size + equal_size + greater_size
            )
    );
}

void assert_three_way_struct(const three_way_struct& way_struct, std::tuple<size_t, size_t, size_t> step) {
    CHECK(way_struct.less_end == std::get<0>(step));
    CHECK(way_struct.equal_end == std::get<1>(step));
    CHECK(way_struct.greater_end == std::get<2>(step));   
}

TEST_CASE( "testcase_generator works", "[knuth_partition]" ) {
    std::vector<int> vector;
    auto [pivot, way_struct] = generate_testcase(1, 2, 1, vector);

    CHECK(pivot == 20);
    assert_three_way_struct(way_struct, { 1, 3, 4 });
    CHECK(vector == std::vector<int>{ 10, 20, 20, 30 });
}

TEST_CASE( "three_way_struct insert accepts bigger element", "[knuth_partition]" ) {
    std::vector<int> vector;
    auto [pivot, way_struct] = generate_testcase(1, 2, 1, vector);

    vector.push_back(pivot+1);
    way_struct = way_struct.accept_element();

    CHECK(vector == std::vector<int>{ 10, 20, 20, 30, 21 });
    assert_three_way_struct(way_struct, { 1, 3, 5 });
}

TEST_CASE( "three_way_struct insert accepts equal element", "[knuth_partition]" ) {
    std::vector<int> vector;
    auto [pivot, way_struct] = generate_testcase(1, 2, 1, vector);

    vector.push_back(pivot);
    way_struct = way_struct.accept_element();

    CHECK(vector == std::vector<int>{ 10, 20, 20, 20, 30 });
    assert_three_way_struct(way_struct, { 1, 4, 5 });
}

TEST_CASE( "three_way_struct insert accepts less element", "[knuth_partition]" ) {
    std::vector<int> vector;
    auto [pivot, way_struct] = generate_testcase(1, 2, 1, vector);

    vector.push_back(pivot-1);
    way_struct = way_struct.accept_element();

    CHECK(vector == std::vector<int>{ 10, 19, 20, 20, 30 });
    assert_three_way_struct(way_struct, { 2, 4, 5 });
}

TEST_CASE( "knuth_partition works", "[knuth_partition]" ) {
    std::vector<int> v { 20, 10, 20, 15, 30 };
    knuth_partition_algorithm algorithm;
    size_t pivot_location = algorithm.partition(vector_view{&v});

    CHECK(pivot_location == 2);
    CHECK(v == std::vector<int> {10, 15, 20, 20, 30});
}