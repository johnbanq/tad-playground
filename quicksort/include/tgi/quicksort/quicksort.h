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

    const std::vector<int>& array() const {
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
            //1st element as pivot, included in le array
            return {v, v.from, v.to};
        }

        basic_partition_step set_state(size_t le_end, size_t ge_begin) const {
            return {v, le_end, ge_begin};
        }

        std::variant<basic_partition_step, size_t> step() const {
            auto ge_begin = this->ge_begin;
            auto le_end = this->le_end;
            const auto& pivot = v.array()[v.from];

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

struct knuth_partition_algorithm: public partition_algorithm {

    struct three_way_struct {
        
        vector_view v;

        size_t less_end; 
        size_t equal_end;
        size_t greater_end;

        static three_way_struct initial(vector_view v) {
            //we consider the 1st element as pivot
            return {v, v.from, v.from+1, v.from+1};
        }

        three_way_struct set_state(size_t less_end, size_t equal_end, size_t greater_end) const {
            return {v, less_end, equal_end, greater_end};
        }

        three_way_struct accept_element() {
            size_t less_end = this->less_end; 
            size_t equal_end = this->equal_end;
            size_t greater_end = this->greater_end;
            auto pivot = v.array()[equal_end-1];
            auto elem = v.array()[greater_end];
            
            do {
                greater_end++;
                if(elem > pivot) {
                    break;
                }

                std::swap(v.array()[equal_end], v.array()[greater_end-1]);
                equal_end++;
                if(elem == pivot) {
                    break;
                }

                std::swap(v.array()[less_end], v.array()[equal_end-1]);
                less_end++;
            } while(0);

            return three_way_struct{ v, less_end, equal_end, greater_end };
        }

    };

    size_t partition(vector_view v) const override {
        auto way_struct = three_way_struct::initial(v);
        while(way_struct.greater_end<v.to) {
            way_struct = way_struct.accept_element();
        }
        return way_struct.less_end;
    }

};


std::vector<int> quicksort(std::vector<int> array);

std::vector<int> quicksort(std::vector<int> array, const partition_algorithm* algorithm);


#endif