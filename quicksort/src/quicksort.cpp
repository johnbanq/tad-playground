#include <algorithm>
#include "tgi/quicksort/quicksort.h"

size_t fake_quicksort_partition(vector_view v) {
    std::sort(v.array().begin()+v.from, v.array().begin()+v.to);
    return ((v.to - v.from) / 2) + v.from;
}

std::variant<basic_partition_step, size_t> basic_partition_step::step() {
    auto ge_begin = this->ge_begin;
    auto le_end = this->le_end;
    auto& pivot = v.array()[v.from];

    while(le_end+1 < ge_begin && v.array()[le_end+1] <= pivot) {
        le_end++;
    } 
    while(ge_begin-1 > le_end && v.array()[ge_begin-1] >= pivot) {
        ge_begin--;
    }

    const auto le_size = (le_end - v.from + 1);
    const auto ge_size = (v.to - ge_begin);
    const auto total_size = v.to - v.from;
    if(le_size + ge_size == total_size) {
        const auto pivot = le_end;
        return pivot;
    } else {
        return basic_partition_step { v, le_end, ge_begin };
    }
}

size_t basic_quicksort_partition(vector_view v) {
    basic_partition_step step = basic_partition_step::initial(v);
    while(true) {
        auto result = step.step();
        if(std::holds_alternative<size_t>(result)) {
            auto pivot = std::get<size_t>(result);
            std::swap(v.array()[v.from], v.array()[pivot]);
            return pivot;
        } else {
            step = std::get<basic_partition_step>(result);
            std::swap(v.array()[step.le_end+1], v.array()[step.ge_begin-1]);
        }
    }
}

void quicksort_recursive(vector_view v) {
    if(!v.is_empty()) {
        auto pivot = fake_quicksort_partition(v);
        quicksort_recursive({v.array_ptr, v.from, pivot});
        quicksort_recursive({v.array_ptr, pivot + 1, v.to});
    }
}

std::vector<int> quicksort(std::vector<int> array) {
    quicksort_recursive({&array, 0, array.size()});
    return array;
}