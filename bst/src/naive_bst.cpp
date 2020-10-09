#include "tgi/bst/naive_bst.h"

#include<iostream>


using node = bst::node;

struct literal_parser {

    bool try_consume(char ch) {
        if(literal[offset]==ch) {
            offset += 1;
            return true;
        } else {
            return false;   
        }
    }

    void consume(char ch) {
        if(!try_consume(ch)) {
            throw std::invalid_argument{std::string{"unexpected charactor ["}+literal[offset]+"] found at literal offset="+std::to_string(offset)+", expecting char"+ch};    
        }
    }

    bool try_consume(const std::string& str) {
        if(literal.substr(offset, str.size())==str) {
            offset += str.size();
            return true;
        } else {
            return false;   
        }
    }

    void consume(const std::string& str) {
        if(!try_consume(str)) {
            throw std::invalid_argument{std::string{"unexpected charactor ["}+literal[offset]+"] found at literal offset="+std::to_string(offset)+", expecting string "+str};    
        }
    }

    int parse_int() {
        size_t parsed = 0;
        int value = std::stoi(literal.substr(offset, literal.size()), &parsed, 10);
        offset += parsed;
        return value;
    }
    
    const std::string& literal;
    size_t offset;

};


node* parse(literal_parser& parser);

bst from_literal(const std::string& literal) {
    literal_parser parser{literal, 0};
    return bst{parse(parser)};
}

node* parse(literal_parser& parser) {
    if(parser.try_consume("null")) {
        return nullptr;
    } else {
        parser.consume('(');
        int value = parser.parse_int();
        if(parser.try_consume(')')) {
            return new node{value, nullptr, nullptr};
        } else {
            parser.consume(':');
            auto left = parse(parser);
            parser.consume(':');
            auto right = parse(parser);
            parser.consume(')');
            return new node{value, left, right};
        }
    }
}


void write(const node* root, std::string& str);

std::string to_literal(const bst& tree) {
    std::string str;
    write(tree.root, str);
    return str;
}

void write(const node* root, std::string& str) {
    if(root == nullptr) { // null root case
        str += "null";
    } 
    
    if(root->left == nullptr && root->right == nullptr) { // 0 child case
        str += '(';
        str += std::to_string(root->value);
        str += ')';
    } else if(root->left == nullptr ^ root->right == nullptr) { // 1 child case
        str += "(";
        str += std::to_string(root->value);
        str += ":";
        if(root->left!=nullptr) {
            write(root->left, str);
        } else {
            str += "null";
        }
        str += ":";
        if(root->right!=nullptr) {
            write(root->right, str);
        } else {
            str += "null";
        }
        str += ")";
    } else { // 2 child case
        str += "(";
        str += std::to_string(root->value);
        str += ":";
        write(root->left, str);
        str += ":";
        write(root->right, str);
        str += ")";
    }
}