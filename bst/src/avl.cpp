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
