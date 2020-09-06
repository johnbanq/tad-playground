#ifndef TGI_SUBSTR_BRUTEFORCE_H_
#define TGI_SUBSTR_BRUTEFORCE_H_

#include <cstdint>
#include <string_view>

int substr_rabinkarp(std::string_view source, std::string_view pattern);

int64_t hash_range(std::string_view range);

#endif