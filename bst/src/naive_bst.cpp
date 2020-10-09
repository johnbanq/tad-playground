#include "tgi/bst/naive_bst.h"

#include<iostream>

using node = bst::node;

node* parse(const std::string& literal, int& offset);

bst from_literal(const std::string& literal) {
    int offset = 0;
    return bst{parse(literal, offset)};
}

void consume(const std::string& literal, int& offset, char ch) {
    if(literal[offset]==ch) {
        offset += 1;
    } else {
        throw std::invalid_argument{std::string{"unexpected charactor ["}+literal[offset]+"] found at literal offset="+std::to_string(offset)+", expecting "+ch};    
    }
}

node* parse(const std::string& literal, int& offset) {
    if(literal.substr(offset, sizeof("null")-1) == "null") {
        offset += 4;
        return nullptr;
    } else {
        if(literal[offset] == '(') {
            offset += 1;
            size_t parsed = 0;
            int value = std::stoi(literal.substr(offset, literal.size()), &parsed, 10);
            offset += parsed;
            if(literal[offset]==')') {
                offset += 1;
                return new node{value, nullptr, nullptr};
            } else {
                if(literal[offset]==':') {
                    offset += 1;
                    auto left = parse(literal, offset);
                    if(literal[offset]==':') {
                        offset += 1;
                        auto right = parse(literal, offset);
                        if(literal[offset]==')') {
                            offset += 1;
                            return new node{value, left, right};
                        } else {
                            throw std::invalid_argument{std::string{"unexpected charactor ["}+literal[offset]+"] found at literal offset="+std::to_string(offset)+", expecting )"};    
                        }
                    } else {
                        throw std::invalid_argument{std::string{"unexpected charactor ["}+literal[offset]+"] found at literal offset="+std::to_string(offset)+", expecting :"};
                    }
                } else {
                    throw std::invalid_argument{std::string{"unexpected charactor ["}+literal[offset]+"] found at literal offset="+std::to_string(offset)+", expecting :"};
                }
            }
        } else {
            throw std::invalid_argument{std::string{"unexpected charactor ["}+literal[offset]+"] found at literal offset="+std::to_string(offset)+", expecting ("};
        }
    }
}

