#include <algorithm>

#include "tgi/bst/rbtree.h"
#include "tgi/bst/internal_rbtree.h"


using node = rbtree::node;


rbtree::rbtree(rbtree&& other)
    :root(nullptr) {
    this->root = other.root;
    other.root = nullptr;
}

rbtree& rbtree::operator=(rbtree&& other) {
    delete_tree(this->root);
    this->root = other.root;
    other.root = nullptr;
    return *this;
}

rbtree::~rbtree() {
    delete_tree(root);
}


struct rbtree_literal_parser: public literal_parser<rbtree::node, std::pair<int, rbtree::color>> {

    rbtree_literal_parser(const std::string& literal, size_t offset)
        : literal_parser<rbtree::node, std::pair<int, rbtree::color>>(literal, offset) {}

    virtual std::pair<int, rbtree::color> parse_value() override {
        auto i = parse_int();
        rbtree::color color;
        if(try_consume('R')) {
            color = rbtree::color::red;
        } else if(try_consume('B')) {
            color = rbtree::color::black;
        } else {
            fail("char R or B");
        }
        return std::make_pair(i, color);
    }

    rbtree::node* build_node(std::pair<int, rbtree::color> value, rbtree::node* parent) override {
        return new rbtree::node{value.first, value.second, parent, nullptr, nullptr};
    }

    rbtree::node* set_child(rbtree::node* node, rbtree::node* left, rbtree::node* right) override {
        node->left = left;
        node->right = right;
        return node;
    }

};

rbtree rbtree_from_literal(const std::string& literal) {
    rbtree_literal_parser parser{literal, 0};
    return rbtree{parser.parse(nullptr)};
}