#ifndef TGI_QUICKSORT_QUICKSORT_H_
#define TGI_QUICKSORT_QUICKSORT_H_

#include <vector>

std::vector<int> quicksort(std::vector<int> array);

// internals //

struct vector_view {

    std::vector<int>& array;
    const size_t from;
    const size_t to;

    bool is_empty() {
        return from >= to;
    }

};

size_t fake_quicksort_partition(vector_view v);

size_t basic_quicksort_partition(vector_view v);

struct basic_partition_step {
    size_t le_end; 
    size_t ge_begin;
};

struct basic_partition_step_result {
    bool finished;
    union {
        basic_partition_step step;
        size_t pivot;
    } data ;
};

basic_partition_step_result basic_quicksort_partition_step(vector_view v, basic_partition_step step);

#endif