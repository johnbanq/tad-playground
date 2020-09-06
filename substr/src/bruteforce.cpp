#include "tgi/substr/bruteforce.h"

int substr_bruteforce(std::string_view source, std::string_view pattern) {
    for (auto i = 0; i <= (int)source.size() - (int)pattern.size(); i++) {
        if(std::equal(source.begin()+i, source.begin()+i+pattern.size(), pattern.begin())) {
            return i;
        }
    }
    return -1;
}
