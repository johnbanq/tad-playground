#include <algorithm>
#include "tgi/quicksort/quicksort.h"

struct vector_view {
    std::vector<int>& array;
    const size_t from;
    const size_t to;
};

size_t quicksort_partition(vector_view v) {
    std::sort(v.array.begin()+v.from, v.array.begin()+v.to);
    return ((v.to - v.from) / 2) + v.from;
}

void quicksort_recursive(std::vector<int>& array, size_t from, size_t to) {
    if(from<to) {
        auto pivot = quicksort_partition({ array, from, to});
        quicksort_recursive(array, from, pivot);
        quicksort_recursive(array, pivot + 1, to);
    }
}

std::vector<int> quicksort(std::vector<int> array) {
    quicksort_recursive(array, 0, array.size());
    return array;
}