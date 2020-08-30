#include <algorithm>
#include "tgi/quicksort/quicksort.h"

size_t quicksort_partition(std::vector<int>& array, size_t from, size_t to) {
    std::sort(array.begin()+from, array.begin()+to);
    return (to - from) / 2;
}

void quicksort_recursive(std::vector<int>& array, size_t from, size_t to) {
    if(from<to) {
        auto pivot = quicksort_partition(array, from, to);
        quicksort_recursive(array, from, pivot);
        quicksort_recursive(array, pivot, to);
    }
}

std::vector<int> quicksort(std::vector<int> array) {
    quicksort_recursive(array, 0, array.size());
    return array;
}