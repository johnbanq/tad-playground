#ifndef TGI_QUICKSORT_QUICKSORT_H_
#define TGI_QUICKSORT_QUICKSORT_H_

#include <variant>
#include <vector>

std::vector<int> quicksort(std::vector<int> array);

// internals //

struct vector_view {

    std::vector<int>* array_ptr;
    size_t from;
    size_t to;

    vector_view(std::vector<int>* array) 
        :array_ptr(array), from(0), to(array->size()){}

    vector_view(std::vector<int>* array, const size_t from, const size_t to) 
        :array_ptr(array), from(from), to(to){}

    bool is_empty() {
        return from >= to;
    }

    std::vector<int>& array() {
        return *array_ptr;
    }

};

size_t fake_quicksort_partition(vector_view v);

size_t basic_quicksort_partition(vector_view v);

struct basic_partition_step {
    
    vector_view v;
    size_t le_end; 
    size_t ge_begin;

    static basic_partition_step initial(vector_view v) {
        return {v, v.from, v.to};
    }

    basic_partition_step set_state(size_t le_end, size_t ge_begin) {
        return {v, le_end, ge_begin};
    }

    std::variant<basic_partition_step, size_t> step();

};

#endif