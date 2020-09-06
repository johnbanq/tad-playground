#include "tgi/substr/bruteforce.h"

int substr_bruteforce(std::string_view source, std::string_view pattern) {
    for (auto i = 0; i < source.size(); i++) {
        if(pattern_match_from_begin(std::string_view{source.data()+i, source.size()-i}, pattern)) {
            return i;
        }
    }
    return -1;
}

bool pattern_match_from_begin(std::string_view source, std::string_view pattern) {
    if(source.size() < pattern.size()) {
        return false;
    }

    auto source_now = source.begin();
    auto pattern_now = pattern.begin();
    while(source_now != source.end() && pattern_now != pattern.end()) {
        if(*source_now!=*pattern_now) {
            return false;
        } else {
            source_now++;
            pattern_now++;
        }
    }

    return true;
}

