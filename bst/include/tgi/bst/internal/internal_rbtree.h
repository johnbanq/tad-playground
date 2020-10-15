#ifndef TGI_BST_INTERNAL_RBTREE_H_
#define TGI_BST_INTERNAL_RBTREE_H_

#include "tgi/bst/rbtree.h"
#include "tgi/bst/hex_util.h"
#include "tgi/bst/internal/internal_bst_common.h"

static std::string rbtree_node_stmt(const rbtree::node* n) {
    return std::to_string(n->value)
    +"[label=\""+std::to_string(n->value)+"("+addr_string(n)+")\"]"
    +"[color="+ (n->color == rbtree::color::red ? "red": "black") +"]"
    +";";
}


template<typename lambda>
void enumerate_node_to_leaf_path_internal(const rbtree::node* node, std::vector<const rbtree::node*>& path, lambda& lam) {
    path.push_back(node);
    if(node == nullptr) {
        lam(path);
    } else {
        enumerate_node_to_leaf_path_internal(node->left, path, lam);
        enumerate_node_to_leaf_path_internal(node->right, path, lam);
    }
    path.pop_back();
}

/**
 * \brief enumerate all paths from node to leaf(nullptr), the lambda accepts const std::vector<rbtree::node*>& for seeing all enumerations 
 */
template<typename lambda>
void enumerate_node_to_leaf_path(const rbtree::node* node, lambda lam) {
    std::vector<const rbtree::node*> path;
    enumerate_node_to_leaf_path_internal(node, path, lam);
}


template<typename lambda>
void enumerate_node_preorder_internal(const rbtree::node* node, lambda& lam) {
    if(node!=nullptr) {
        lam(node);
        enumerate_node_preorder_internal(node->left, lam);
        enumerate_node_preorder_internal(node->right, lam);
    }
}

template<typename lambda>
void enumerate_node_preorder(const rbtree::node* node, lambda lam) {
    enumerate_node_preorder_internal(node, lam);
}

#endif