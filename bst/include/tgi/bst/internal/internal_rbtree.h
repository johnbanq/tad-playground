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


void rbtree_left_rotate(rbtree::node*& node);

void rbtree_right_rotate(rbtree::node*& node);


/**
 * \brief perform 1 round of insert fix up, 
 *     we assume to_fix is a red node that might have broken the rules caused by insert of previous execution of the fixup
 *     we fix it here and return the next node you might need to fix, nullptr if all done
 */
rbtree::node* perform_insert_fixup(rbtree& tree, rbtree::node* to_fix);

/**
 * \brief perform 1 round of delete fix up, 
 *     we assume to_fix is a node having a "floating black" on it caused by removing a black node in deletion or by previous execution of the fixup
 *     we fix it here and return the next node you might need to fix, nullptr if all done
 * \note we need the root of parent since to_fix might be nullptr
 */
rbtree::node* perform_delete_fixup(rbtree& tree, rbtree::node* parent, rbtree::node* to_fix);

#endif