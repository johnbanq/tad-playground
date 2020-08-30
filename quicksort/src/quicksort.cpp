#include <algorithm>
#include "tgi/quicksort/quicksort.h"

size_t fake_quicksort_partition(vector_view v) {
    std::sort(v.array.begin()+v.from, v.array.begin()+v.to);
    return ((v.to - v.from) / 2) + v.from;
}

basic_partition_step_result basic_quicksort_partition_step(vector_view v, basic_partition_step step) {
    auto ge_begin = step.ge_begin;
    auto le_end = step.le_end;
    auto& pivot = v.array[v.from];

    while(le_end+1 < ge_begin && v.array[le_end+1] <= pivot) {
        le_end++;
    } 
    while(ge_begin-1 > le_end && v.array[ge_begin-1] >= pivot) {
        ge_begin--;
    }

    const auto le_size = (le_end - v.from + 1);
    const auto ge_size = (v.to - ge_begin);
    const auto total_size = v.to - v.from;
    if(le_size + ge_size == total_size) {
        basic_partition_step_result result;
        result.finished = true;
        result.data.pivot = le_end;
        return result;
    } else {
        basic_partition_step_result result;
        result.finished = false;
        result.data.step.le_end = le_end;
        result.data.step.ge_begin = ge_begin;
        return result;
    }
}

size_t basic_quicksort_partition(vector_view v) {
    basic_partition_step now;
    now.le_end = v.from;
    now.ge_begin = v.to + 1;
    while(true) {
        auto result = basic_quicksort_partition_step(v, now);
        if(result.finished) {
            std::swap(v.array[v.from], v.array[result.data.pivot]);
            return result.data.pivot;
        } else {
            now = result.data.step;
            std::swap(v.array[now.le_end+1], v.array[now.ge_begin-1]);
        }
    }
}

void quicksort_recursive(vector_view v) {
    if(!v.is_empty()) {
        auto pivot = fake_quicksort_partition(v);
        quicksort_recursive({v.array, v.from, pivot});
        quicksort_recursive({v.array, pivot + 1, v.to});
    }
}

std::vector<int> quicksort(std::vector<int> array) {
    quicksort_recursive({array, 0, array.size()});
    return array;
}