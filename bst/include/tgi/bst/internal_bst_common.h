#ifndef TGI_BST_INTERNAL_BST_COMMON_H_
#define TGI_BST_INTERNAL_BST_COMMON_H_

#include<utility>
#include<string>
#include<vector>
#include<limits>

#include "tgi/bst/hex_util.h"

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

// graphviz helper //

template<typename node_type>
struct graphviz_writer {

    virtual ~graphviz_writer() = default;

    std::string write(const node_type* root) {
        std::string buffer;
        buffer += "digraph bst{";
        write(root, buffer);
        buffer += "}";
        return buffer;
    }

    void write(const node_type* root, std::string& buffer) {
        if(root!=nullptr) {
            write_node(root, buffer);
            if(root->parent != nullptr) {
                write_edge(root, root->parent, "parent", buffer);
            }
            if(root->left != nullptr) {
                write_edge(root, root->left, "left", buffer);
            }
            if(root->right != nullptr) {
                write_edge(root, root->right, "right", buffer);
            }
            write(root->left, buffer);
            write(root->right, buffer);
        }
    }

    virtual void write_node(const node_type* root, std::string& buffer) = 0;

    virtual void write_edge(const node_type* from, const node_type* to, const std::string& type, std::string& buffer) = 0;

};

/**
 * generic write function for write_node
 */
static std::string node_stmt(int value, const void* addr) {
    return std::to_string(value)+"[label=\""+std::to_string(value)+"("+int_to_hex((uint16_t(addr)))+")\"];";
}

/**
 * generic write function for write_edge
 */
static std::string edge_stmt(int from, int to, const std::string& edge) {
    auto stmt = std::to_string(from)+" -> "+std::to_string(to)+" [label=\""+edge+"\"]";
    if(edge == "parent") {
        stmt += "[color=grey][fontcolor=grey]";
    }
    stmt += ";";
    return stmt;
}

// violation check util //

/**
 * find violations in pointer pointing, eg: incorrect parent
 */
template<typename node_type>
void find_pointer_violation_on_node(const node_type* node, std::vector<std::string>& violations) {
    if(node != nullptr) {
        if(node->left!=nullptr && node->left->parent != node) {
            violations.push_back(parent_violation(node, node->left));
        }
        find_pointer_violation_on_node(node->left, violations);
        if(node->right!=nullptr && node->right->parent != node) {
            violations.push_back(parent_violation(node, node->right));
        }
        find_pointer_violation_on_node(node->right, violations);
    }
}

/**
 * same as on_node variant, but starts from tree root and will check root's parent
 */
template<typename node_type>
void find_pointer_violation(const node_type* root, std::vector<std::string>& violations) {
    if(root != nullptr && root->parent != nullptr) {
        violations.push_back("root's parent must be null!");
    }
    find_pointer_violation_on_node(root, violations);
}

template<typename node_type>
static std::string parent_violation(const node_type* parent, const node_type* child) {
    return "node {"+std::to_string(child->value)+"}'s parent should point to {"+int_to_hex((uint16_t)parent)+"}, not {"+int_to_hex((uint16_t)child->parent)+"}";
}


/**
 * find violation in value placement, eg bigger value in left branch
 * the node is expected to be in range (min, max]
 */
template<typename node_type>
void find_value_violation_on_node(const node_type* node, int min, int max, std::vector<std::string>& violations) {
    if(node!=nullptr) {
        if(!(min < node->value && node->value <= max)) {
            auto min_repr = (min == std::numeric_limits<int>::min() ? std::string{"int_min"} : std::to_string(min));
            auto max_repr = (max == std::numeric_limits<int>::max() ? std::string{"int_max"} : std::to_string(max));
            violations.push_back("{"+std::to_string(node->value)+"} is in wrong place: it should must be in range ("+min_repr+","+max_repr+"]");
        } else {
            find_value_violation_on_node(node->left, min, node->value, violations);
            find_value_violation_on_node(node->right, node->value, max, violations);
        }
    }
}

template<typename node_type>
void find_value_violation(const node_type* root, std::vector<std::string>& violations) {
    return find_value_violation_on_node(root, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), violations);
}

#endif