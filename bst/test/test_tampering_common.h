#ifndef TGI_BST_TEST_TAMPERING_COMMON_H_
#define TGI_BST_TEST_TAMPERING_COMMON_H_

#include <vector>
#include <set>
#include <random>
#include <algorithm>
#include <numeric>
#include <execution>
#include <iostream>
#include "catch.hpp"

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

std::vector<int> shuffle(std::vector<int> vector) {
    std::random_device r;
    std::default_random_engine e1(r());
    std::shuffle(vector.begin(), vector.end(), e1);
    return vector;
}

template<typename lam>
void run_n_times(std::execution::parallel_policy policy, int n, lam lambda) {
    std::vector<int> times(n);
    std::iota(times.begin(), times.end(), 0);
    std::for_each(
        std::execution::par,
        times.begin(), times.end(), 
        [=](int unused){
            lambda();
        }
    );
}

template<typename lam>
void run_n_times(std::execution::sequenced_policy policy, int n, lam lambda) {
    for(int i=0;i<n;i++) {
        lambda();
    }
}

template<typename tree_type, typename violation_config_type>
void perform_insertion_test(int length_of_experiment, violation_config_type config) {
    tree_type tree;
    std::vector<int> insert_history;
    std::set<int> expected_elements;
    for_random_int(length_of_experiment, -1000, 1000, [&](int value) {
        insert_history.push_back(value);
        expected_elements.insert(value);

        try {
            insert(tree, value);
        } catch(std::exception& e) {
            INFO("error in performing insertion");
            INFO("inserting: "+std::to_string(value));
            //this is to help debugging and visualization of related data
            CHECK(insert_history == std::vector<int>{});
            throw e;
        }
                
        auto violations = find_violation(tree, config);
        if(violations.size()!=0) {
            INFO("insertion violated tree constraints");
            INFO("inserting: "+std::to_string(value));
            //this is to help debugging and visualization of related data
            CHECK(insert_history == std::vector<int>{});
            REQUIRE(violations == std::vector<std::string>{});
        }
    });

    REQUIRE(count(tree)==expected_elements.size());
    auto elems = list_all(tree);
    REQUIRE(std::set<int>(elems.begin(), elems.end())==expected_elements);
}

template<typename tree_type, typename violation_config_type>
void perform_deletion_test(int length_of_experiment, violation_config_type config) {
    tree_type tree;
    //the shadow represent the state of the tree before action, used on error report
    tree_type last_correct;

    std::set<int> expected_elements;
    std::vector<int> insert_history;
    for_random_int(length_of_experiment, -1000, 1000, [&](int value){
        expected_elements.insert(value);
        insert_history.push_back(value);
        insert(tree, value);
        insert(last_correct, value);
    });
    
    std::vector<int> remove_history;
    for (auto to_remove : shuffle(insert_history)) {
        try {
            remove(tree, to_remove);
        } catch(std::exception& e) {
            INFO("error in performing remove");
            INFO("removing: "+std::to_string(to_remove));
            //this is to help debugging and visualization of related data
            INFO("tree: "+to_graphviz(tree));
            INFO("last_correct: "+to_graphviz(last_correct));
            CHECK(insert_history == std::vector<int>{});
            CHECK(remove_history == std::vector<int>{});
            throw e;
        }
    
        auto violations = find_violation(tree, config);
        if(violations.size()!=0) {
            INFO("deletion violated tree constraints");
            INFO("removing: "+std::to_string(to_remove));
            //this is to help debugging and visualization of related data
            INFO("tree: "+to_graphviz(tree));
            INFO("last_correct: "+to_graphviz(last_correct));
            CHECK(insert_history == std::vector<int>{});
            CHECK(remove_history == std::vector<int>{});
            REQUIRE(violations == std::vector<std::string>{});
        }

        remove(last_correct, to_remove);
        remove_history.push_back(to_remove);
    }
    REQUIRE(count(tree)==0);
}

template<typename tree_type>
void recreate_deletion_test(
    std::vector<int> insert_history,
    std::vector<int> remove_history,
    int removing
) {
    tree_type tree;
    tree_type last_correct;

    for(auto value : insert_history) {
        insert(tree, value);
        insert(last_correct, value);
    }

    for(auto&value: remove_history) {
        remove(tree, value);
        remove(last_correct, value);
    }
    
    try {
        remove(tree, removing);
    } catch(std::exception& e) {
        INFO("error in performing remove");
        INFO("removing: "+std::to_string(removing));
        //this is to help debugging and visualization of related data
        INFO("tree: "+to_graphviz(tree));
        INFO("last_correct: "+to_graphviz(last_correct));
        throw e;
    }

    auto violations = find_violation(tree);
    if(violations.size()!=0) {
        INFO("deletion violated tree constraints");
        INFO("removing: "+std::to_string(removing));
        //this is to help debugging and visualization of related data
        INFO("tree: "+to_graphviz(tree));
        INFO("last_correct: "+to_graphviz(last_correct));
        REQUIRE(violations == std::vector<std::string>{});
    }
}

#endif