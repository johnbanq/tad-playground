#include <algorithm>
#include "tgi/quicksort/quicksort.h"


void quicksort_recursive(vector_view v, const partition_algorithm* algorithm) {
    if(!v.is_empty()) {
        auto pivot = algorithm->partition(v);
        quicksort_recursive({v.array_ptr, v.from, pivot}, algorithm);
        quicksort_recursive({v.array_ptr, pivot + 1, v.to}, algorithm);
    }
}

std::vector<int> quicksort(std::vector<int> array, const partition_algorithm* algorithm) {
    quicksort_recursive({&array, 0, array.size()}, algorithm);
    return array;
}


std::vector<int> quicksort(std::vector<int> array) {
    fake_partition_algorithm algorithm;
    return quicksort(array, &algorithm);
}