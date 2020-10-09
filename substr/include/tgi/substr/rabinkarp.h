#ifndef TGI_SUBSTR_BRUTEFORCE_H_
#define TGI_SUBSTR_BRUTEFORCE_H_

#include <cstdint>
#include <string_view>
#include <iostream>
#include <cassert>

struct hash_slidewindow_iterator {

    static const int64_t HASH_MODULO = 997;

    hash_slidewindow_iterator(std::string_view source, size_t window_size): 
        source(source), 
        window_size(window_size),
        current_offset(0) 
        {
            assert(source.size()>=window_size);
            //compute the initial hash value
            int64_t sum = 0;
            int64_t exp_base = 1;
            std::string_view range{source.data(), window_size};
            for(auto iter = range.rbegin();iter!=range.rend();iter++) {
                sum = (sum + (((*iter)*exp_base) % HASH_MODULO)) % HASH_MODULO;
                exp_base = (exp_base * 256) % HASH_MODULO;
            }
            _hash_value = sum;
        }

    int64_t hash_value() {
        return _hash_value;
    }

    std::string_view window() {
        return std::string_view(source.data()+current_offset, window_size);
    }

    int64_t offset() {
        return current_offset;
    }

    bool has_next_window() {
        return current_offset + window_size < source.size();
    }

    bool try_slide_right() {
        if(!has_next_window()) {
            return false;
        } else {
            //update hash value
            const auto current_hash = hash_value();
            const auto last_digit_complement = (HASH_MODULO - (source[current_offset]*exp_mod(256, window_size-1, HASH_MODULO)) % HASH_MODULO);
            const auto next_hash = ((((current_hash + last_digit_complement) * 256) % HASH_MODULO) + source[current_offset+window_size]) % HASH_MODULO;
            _hash_value = next_hash;
            
            current_offset++;
            return true;
        }
    }

    int64_t exp_mod(int64_t base, int64_t exp, int64_t mod) {
        int64_t exp_base = 1;
        for(int64_t i=0;i<exp;i++) {
            exp_base = (exp_base * base) % mod;
        }
        return exp_base;
    }

private:
    std::string_view source;
    size_t window_size;

    size_t current_offset; 

    int64_t _hash_value;

};

int substr_rabinkarp(std::string_view source, std::string_view pattern);

int64_t hash_range(std::string_view range);

#endif