#include "tgi/bst/naive_bst.h"

#include<iostream>

using node = bst::node;

node* parse(const std::string& literal, int& offset);

bst from_literal(const std::string& literal) {
    int offset = 0;
    return bst{parse(literal, offset)};
}



bool try_consume(const std::string& literal, int& offset, char ch) {
    if(literal[offset]==ch) {
        offset += 1;
        return true;
    } else {
        return false;   
    }
}

void consume(const std::string& literal, int& offset, char ch) {
    if(!try_consume(literal, offset, ch)) {
        throw std::invalid_argument{std::string{"unexpected charactor ["}+literal[offset]+"] found at literal offset="+std::to_string(offset)+", expecting "+ch};    
    }
}

bool try_consume(const std::string& literal, int& offset, const std::string& str) {
    if(literal.substr(offset, str.size())==str) {
        offset += str.size();
        return true;
    } else {
        return false;   
    }
}

void consume(const std::string& literal, int& offset, const std::string& str) {
    if(!try_consume(literal, offset, str)) {
        throw std::invalid_argument{std::string{"unexpected charactor ["}+literal[offset]+"] found at literal offset="+std::to_string(offset)+", expecting string="+str};    
    }
}

int parse_int(const std::string& literal, int& offset) {
    size_t parsed = 0;
    int value = std::stoi(literal.substr(offset, literal.size()), &parsed, 10);
    offset += parsed;
    return value;
}

node* parse(const std::string& literal, int& offset) {
    if(try_consume(literal, offset, "null")) {
        return nullptr;
    } else {
        consume(literal, offset, '(');
        int value = parse_int(literal, offset);
        if(try_consume(literal, offset, ')')) {
            return new node{value, nullptr, nullptr};
        } else {
            consume(literal, offset, ':');
            auto left = parse(literal, offset);
            consume(literal, offset, ':');
            auto right = parse(literal, offset);
            consume(literal, offset, ')');
            return new node{value, left, right};
        }
    }
}

