#ifndef TGI_QUICKSORT_QUICKSORT_H_
#define TGI_QUICKSORT_QUICKSORT_H_

#include <variant>
#include <vector>


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

// algorithms

struct partition_algorithm {

    virtual ~partition_algorithm() = default;

    virtual size_t partition(vector_view v) const = 0;

};

struct fake_partition_algorithm: public partition_algorithm {

    size_t partition(vector_view v) const override {
        std::sort(v.array().begin()+v.from, v.array().begin()+v.to);
        return ((v.to - v.from) / 2) + v.from;
    }

};

struct basic_partition_algorithm: public partition_algorithm {

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

        std::variant<basic_partition_step, size_t> step() {
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

    };

    size_t partition(vector_view v) const override {
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

};


std::vector<int> quicksort(std::vector<int> array);

std::vector<int> quicksort(std::vector<int> array, const partition_algorithm* algorithm);


#endif