#include "tgi/substr/rabinkarp.h"

int substr_rabinkarp(std::string_view source, std::string_view pattern) {
    auto pattern_hash = hash_range(pattern);
    for (auto i = 0; i <= (int)source.size() - (int)pattern.size(); i++) {
        if(hash_range(std::string_view(source.data()+i, pattern.size()))!=pattern_hash) {
            continue;
        }
        if(std::equal(source.begin()+i, source.begin()+i+pattern.size(), pattern.begin())) {
            return i;
        }
    }
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