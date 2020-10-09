#include "tgi/substr/rabinkarp.h"


int substr_rabinkarp(std::string_view source, std::string_view pattern) {
    if(source.size()<pattern.size()) {
        return -1;
    }

    auto pattern_hash = hash_range(pattern);
    auto window_iter = hash_slidewindow_iterator(source, pattern.size());

    do {
        if(window_iter.hash_value()==pattern_hash) {
            return window_iter.offset();
        }
    } while(window_iter.try_slide_right());
    
    return -1;
}

const int64_t HASH_MODULO = 997;

int64_t hash_range(std::string_view range) {
    int64_t sum = 0;
    int64_t exp_base = 1;
    for(auto iter = range.rbegin();iter!=range.rend();iter++) {
        sum = (sum + (((*iter)*exp_base) % HASH_MODULO)) % HASH_MODULO;
        exp_base = (exp_base * 256) % HASH_MODULO;
    }
    return sum;
}