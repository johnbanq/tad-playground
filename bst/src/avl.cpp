#include <algorithm>

#include "tgi/bst/avl.h"
#include "tgi/bst/internal_avl.h"
#include "tgi/bst/internal_naive_bst.h"


using node = avl::node;

void delete_tree(node* n) {
    if(n!=nullptr) {
        delete_tree(n->left);
        delete_tree(n->right);
        delete n;
    }
}

avl::avl(avl&& other)
    :root(nullptr) {
    this->root = other.root;
    other.root = nullptr;
}

avl& avl::operator=(avl&& other) {
    delete_tree(this->root);
    this->root = other.root;
    other.root = nullptr;
    return *this;
}

avl::~avl() {
    delete_tree(root);
}


struct avl_literal_parser: public literal_parser<avl::node> {

    avl_literal_parser(const std::string& literal, size_t offset)
        : literal_parser<avl::node>(literal, offset) {}

    avl::node* build_node(int value, avl::node* parent) override {
        return new avl::node{value, 0, parent, nullptr, nullptr};
    }

    avl::node* set_child(avl::node* node, avl::node* left, avl::node* right) override {
        node->left = left;
        node->right = right;
        node->height = std::max(left==nullptr?-1:left->height, right==nullptr?-1:right->height) + 1;
        return node;
    }

};

avl avl_from_literal(const std::string& literal) {
    avl_literal_parser parser{literal, 0};
    return avl{parser.parse(nullptr)};
}

struct avl_literal_writer: public literal_writer<avl::node> {

    virtual void write_value(const avl::node* root, std::string& str) override {
        str += std::to_string(root->value);
    }

};

std::string to_literal(const avl& tree) {
    avl_literal_writer writer;
    std::string str;
    writer.write(tree.root, str);
    return str;
}

struct avl_graphviz_writer: public graphviz_writer<avl::node> {

    virtual void write_node(const avl::node* node, std::string& buffer) override {
        buffer += avl_node_stmt(node);
    }

    virtual void write_edge(const avl::node* from, const avl::node* to, const std::string& type, std::string& buffer) override {
        buffer += edge_stmt(from->value, to->value, type);
    }

};

std::string to_graphviz(const avl& tree) {
    avl_graphviz_writer writer;
    return writer.write(tree.root);
}

#include<iostream>
/**
 * this function shouts the mismatch of recorded and actual height in tree, and will return real height of the node as reference
 */
int check_height_matches(const node* node, std::vector<std::string>& violations) {
    if(node != nullptr) {
        int left = check_height_matches(node->left, violations);
        int right = check_height_matches(node->right, violations);
        int expected_height = std::max(left, right)+1;
        if(node->height!=expected_height) {
            violations.push_back("{"+std::to_string(node->value)+"} has wrong height="+std::to_string(node->height)+", should be height="+std::to_string(expected_height));
        }
        return expected_height;
    } else {
        //so leaf will have 0
        return -1;
    }
}

/**
 * this function shouts node with bad balance factor (not +1,0,-1)
 */
void check_balance(const node* node, std::vector<std::string>& violations) {
    if(node != nullptr) {
        int left_height = node->left == nullptr ? -1 : node->left->height;
        int right_height = node->right == nullptr ? -1 : node->right->height;
        int factor = left_height - right_height;
        if(std::abs(factor)>1) {
            violations.push_back("{"+std::to_string(node->value)+"} has a bad balance factor of "+std::to_string(factor));
        }
    }
}

std::vector<std::string> find_violation(const avl& tree) {
    auto violations = std::vector<std::string>{};
    find_pointer_violation(tree.root, violations);
    find_value_violation(tree.root, violations);
    if(violations.size() == 0) {
        check_height_matches(tree.root, violations);
        check_balance(tree.root, violations);
    }
    return violations;
}