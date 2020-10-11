#ifndef TGI_BST_INTERNAL_NAIVE_BST_H_
#define TGI_BST_INTERNAL_NAIVE_BST_H_

#include<utility>

#include "tgi/bst/hex_util.h"
#include "tgi/bst/naive_bst.h"

// literal parsing logic // 

template<typename node_type>
struct literal_parser {

    literal_parser(const std::string& literal, size_t offset)
        : literal(literal), offset(offset) {}

    virtual ~literal_parser() = default;


    node_type* parse(node_type* parent) {
        if(try_consume("null")) {
            return nullptr;
        } else {
            consume('(');
            int value = parse_int();
            if(try_consume(')')) {
                auto result = build_node(value, parent);
                return set_child(result, nullptr, nullptr);
            } else {
                auto result = build_node(value, parent);
                consume(':');
                auto left = parse(result);
                consume(':');
                auto right = parse(result);
                consume(')');
                return set_child(result, left, right);
            }
        }
    }

    virtual node_type* build_node(int value, node_type* parent) = 0;

    virtual node_type* set_child(node_type* node, node_type* left, node_type* right) = 0;


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

// others //

/**
 * construct the node statement of graphviz
 */
static std::string node_stmt(int value, const bst::node* addr) {
    return std::to_string(value)+"[label=\""+std::to_string(value)+"("+int_to_hex((uint16_t(addr)))+")\"];";
}

/**
 * construct the edge statement of graphviz
 */
static std::string edge_stmt(int from, int to, const std::string& edge) {
    auto stmt = std::to_string(from)+" -> "+std::to_string(to)+" [label=\""+edge+"\"]";
    if(edge == "parent") {
        stmt += "[color=grey][fontcolor=grey]";
    }
    stmt += ";";
    return stmt;
}


static std::string parent_violation(const bst::node* parent, const bst::node* child) {
    return "node {"+std::to_string(child->value)+"}'s parent should point to {"+int_to_hex((uint16_t)parent)+"}, not {"+int_to_hex((uint16_t)child->parent)+"}";
}

/**
 * locate a pointer that points to the slot which should hold a pointer to the expected node(ref)
 * and the expected parent of the expected node
 */
std::pair<bst::node*, bst::node**> locate_parent_and_expected_ref(bst& tree, int value);

#endif