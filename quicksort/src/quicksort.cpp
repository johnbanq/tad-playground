#include <algorithm>
#include "tgi/quicksort/quicksort.h"

struct vector_view {
    std::vector<int>& array;
    const size_t from;
    const size_t to;

    bool is_empty() {
        return from >= to;
    }

};

size_t quicksort_partition(vector_view v) {
    std::sort(v.array.begin()+v.from, v.array.begin()+v.to);
    return ((v.to - v.from) / 2) + v.from;
}

void quicksort_recursive(vector_view v) {
    if(!v.is_empty()) {
        auto pivot = quicksort_partition(v);
        quicksort_recursive({v.array, v.from, pivot});
        quicksort_recursive({v.array, pivot + 1, v.to});
    }
}

std::vector<int> quicksort(std::vector<int> array) {
    quicksort_recursive({array, 0, array.size()});
    return array;
}