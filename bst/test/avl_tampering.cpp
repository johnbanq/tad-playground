#include "tgi/bst/avl.h"
#include "test_tampering_common.h"

TEST_CASE( "insert 1000 random value 1000 times", "[.][avl][tamper]" ) {
    const int experiment_time = 1000;
    const int length_of_experiment = 1000;

    run_n_times(
        std::execution::par,
        experiment_time, 
        [=](){
            perform_insertion_test<avl>(length_of_experiment, avl_find_violation_config{});
        }
    );
}

TEST_CASE( "remove all element from 1000 random element avl 1000 times", "[.][avl][tamper]" ) {
    const int experiment_time = 1000;
    const int length_of_experiment = 1000;

    run_n_times(
        std::execution::par,
        experiment_time, 
        [=](){
            perform_deletion_test<avl>(length_of_experiment, avl_find_violation_config{});
        }
    );
}