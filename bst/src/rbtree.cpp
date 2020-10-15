#include <algorithm>

#include "tgi/bst/rbtree.h"
#include "tgi/bst/internal_rbtree.h"

template<typename node_type>
void delete_tree(node_type* n) {
    if(n!=nullptr) {
        delete_tree(n->left);
        delete_tree(n->right);
        delete n;
    }
}

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