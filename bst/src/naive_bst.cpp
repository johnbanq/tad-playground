#include "tgi/bst/naive_bst.h"

#include<iostream>

using node = bst::node;

node* parse(const std::string& literal, int offset);

bst from_literal(const std::string& literal) {
    return bst{parse(literal, 0)};
}

node* parse(const std::string& literal, int offset) {
    if(literal == "null") {
        return nullptr;
    } else {
        if(literal[0] == '(') {
            size_t parsed = 0;
            int value = std::stoi(literal.substr(1, literal.size()), &parsed, 10);
            if(literal[0+parsed+1]==')') {
                return new node{value, nullptr, nullptr};
            } else {
                throw std::invalid_argument{std::string{"unexpected charactor ["}+literal[0+parsed+1]+"] found at literal offset="+std::to_string(offset)};
            }
        } else {
            throw std::invalid_argument{std::string{"unexpected charactor ["}+literal[0]+"] found at literal offset="+std::to_string(offset)};
        }
    }
}

