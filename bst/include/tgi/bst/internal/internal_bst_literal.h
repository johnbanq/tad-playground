#ifndef TGI_BST_INTERNAL_BST_LITERAL_H_
#define TGI_BST_INTERNAL_BST_LITERAL_H_

#include<string>

#include "tgi/bst/hex_util.h"

// literal parsing logic //

/**
 * \brief parser class to parse a bst from literal:
 * 
 *  nodeLiteral = "null" | ($VALUE$) | ($VALUE$:nodeLiteral:nodeLiteral)
 * 
 * the parsing of $VALUE$ is the job of parse_value
 * the parsed value and child node will then be fed into build_node to new a node and establish parent-child relation
 * 
 */
template<typename node_type, typename value_type>
struct literal_parser {

    literal_parser(const std::string& literal, size_t offset)
        : literal(literal), offset(offset) {}

    virtual ~literal_parser() = default;


    node_type* parse() {
        if(try_consume("null")) {
            return nullptr;
        } else {
            consume('(');
            auto value = parse_value();
            if(try_consume(')')) {
                return build_node(std::move(value), nullptr, nullptr);
            } else {
                consume(':');
                auto left = parse();
                consume(':');
                auto right = parse();
                consume(')');
                return build_node(std::move(value), left, right);
            }
        }
    }


    virtual value_type parse_value() = 0;

    virtual node_type* build_node(value_type value, node_type* left, node_type* right) = 0;


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
            fail("char "+ch);  
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
            fail("string "+str);
        }
    }

    int parse_int() {
        size_t parsed = 0;
        int value = std::stoi(literal.substr(offset, literal.size()), &parsed, 10);
        offset += parsed;
        return value;
    }

    void fail(const std::string& expected) {
        fail(std::string{"charactor ["}+literal[offset]+"]", expected);
    }

    void fail(const std::string& unexpected, const std::string& expected) {
        throw std::invalid_argument{std::string{"unexpected +"+unexpected+"+ found at literal offset="+std::to_string(offset)+", expecting "+expected}};
    }

    const std::string& literal;
    size_t offset;

};

template<typename node_type>
struct literal_writer {

    virtual ~literal_writer() = default;

    void write(const node_type* root, std::string& str) {
        if(root == nullptr) { // null root case
            str += "null";
        } else {
            str += "(";
            write_value(root, str);
            if(root->left != nullptr || root->right != nullptr) { // at least 1 child
                str += ":";
                write(root->left, str);
                str += ":";
                write(root->right, str);
            }
            str += ")";
        }
    }

    virtual void write_value(const node_type* root, std::string& str) = 0;

};

#endif