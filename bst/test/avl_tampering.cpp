#include <vector>
#include <set>
#include <random>
#include <algorithm>
#include <numeric>
#include <execution>
#include "catch.hpp"

#include "tgi/bst/avl.h"
#include "test_bst_common.h"

template<typename lambda>
void for_random_int(int amount, int min, int max, lambda lam) {
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(min, max);
    for (size_t i = 0; i < amount; i++) {
        lam(uniform_dist(e1));
    }
}

void shuffle(std::vector<int>& vector) {
    std::random_device r;
    std::default_random_engine e1(r());
    std::shuffle(vector.begin(), vector.end(), e1);
}

TEST_CASE( "insert 1000 random value 1000 times", "[.][avl][tamper]" ) {
    const int experiment_time = 1000;
    const int length_of_experiment = 1000;

    std::vector<int> times(experiment_time);
    std::iota(times.begin(), times.end(), 0);

    std::for_each(
        std::execution::par,
        times.begin(), times.end(), 
        [=](int unused){
            avl tree;
            std::vector<int> inserted;
            std::set<int> expected_elements;
            for_random_int(length_of_experiment, -1000, 1000, [&](int value){
                inserted.push_back(value);
                expected_elements.insert(value);

                try {
                    insert(tree, value);
                } catch(std::exception& e) {
                    INFO("error in performing insertion");
                    INFO("inserting: "+std::to_string(value));
                    //this is to help debugging and visualization of related data
                    CHECK(inserted == std::vector<int>{});
                    throw e;
                } 
                
                auto violations = find_violation(tree);
                if(violations.size()!=0) {
                    INFO("insertion violated avl constraints");
                    INFO("inserting: "+std::to_string(value));
                    //this is to help debugging and visualization of related data
                    CHECK(inserted == std::vector<int>{});
                    REQUIRE(violations == std::vector<std::string>{});
                }
            });
            REQUIRE(count(tree)==expected_elements.size());
            auto elems = list_all(tree);
            REQUIRE(std::set<int>(elems.begin(), elems.end())==expected_elements);
        }
    );
    
}

#include<iostream>
TEST_CASE( "remove all element from 1000 random element avl 1000 times", "[.][avl][tamper]" ) {
    const int experiment_time = 1000;
    const int length_of_experiment = 1000;

    std::vector<int> times(experiment_time);
    std::iota(times.begin(), times.end(), 0);

    std::for_each(
        std::execution::par,
        times.begin(), times.end(), 
        [=](int unused){
            avl tree;
            std::set<int> expected_elements;
            std::vector<int> inserted;
            for_random_int(length_of_experiment, -1000, 1000, [&](int value){
                expected_elements.insert(value);
                inserted.push_back(value);
                insert(tree, value);
            });
            
            std::vector<int> removed;
            std::vector<int> remove_order(inserted.begin(), inserted.end());
            shuffle(remove_order);
            for (auto to_remove : remove_order) {
                removed.push_back(to_remove);
                try {
                    remove(tree, to_remove);
                } catch(std::exception& e) {
                    INFO("error in performing remove");
                    INFO("removing: "+std::to_string(to_remove));
                    //this is to help debugging and visualization of related data
                    avl last_correct;
                    for_each(inserted.begin(), inserted.end(), [&](int v){insert(last_correct, v);});
                    for_each(removed.begin(), (removed.begin()+(removed.size()-1)), [&](int v){remove(last_correct, v);});

                    std::cout<<(count(last_correct))<<std::endl;
                    std::cout<<(to_graphviz(last_correct))<<std::endl;
                    CHECK(removed == std::vector<int>{});
                    CHECK(expected_elements == std::set<int>{});
                    throw e;
                }         
            
                auto violations = find_violation(tree);
                if(violations.size()!=0) {
                    INFO("insertion violated avl constraints");
                    INFO("removing: "+std::to_string(to_remove));
                    //this is to help debugging and visualization of related data
                    CHECK(removed == std::vector<int>{});
                    REQUIRE(violations == std::vector<std::string>{});
                }
            }
            REQUIRE(count(tree)==0);
        });
}