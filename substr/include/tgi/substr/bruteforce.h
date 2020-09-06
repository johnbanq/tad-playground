#ifndef TGI_SUBSTR_BRUTEFORCE_H_
#define TGI_SUBSTR_BRUTEFORCE_H_

#include <string_view>

int substr_bruteforce(std::string_view source, std::string_view pattern);

bool pattern_match_from_begin(std::string_view source, std::string_view pattern);


#endif