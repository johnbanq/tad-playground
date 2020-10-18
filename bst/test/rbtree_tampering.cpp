#include "tgi/bst/rbtree.h"
#include "test_tampering_common.h"

TEST_CASE( "insert 1000 random value 1000 times into rbtree", "[rbtree][tamper]" ) {
    const int experiment_time = 1000;
    const int length_of_experiment = 1000;

    run_n_times(
        std::execution::par,
        experiment_time, 
        [=](){
            rbtree_find_violation_config config;
            config.fast_path_check = true;
            perform_insertion_test<rbtree>(length_of_experiment, config);
        }
    );
}

TEST_CASE( "remove regression #1", "[rbtree][tamper]" ) {
    recreate_deletion_test<rbtree>(
        {-631, 3, -739, -522, -171, -501, 26, 61, 502, 935},
        {},
        -171
    );
}

TEST_CASE( "insert 1000 random element to rbtree and remove in new random order 1000 times", "[rbtree][tamper]" ) {
    const int experiment_time = 1000;
    const int length_of_experiment = 1000;

    run_n_times(
        std::execution::par,
        experiment_time, 
        [=](){
            rbtree_find_violation_config config;
            config.fast_path_check = true;
            perform_deletion_test<rbtree>(length_of_experiment, config);
        }
    );
}