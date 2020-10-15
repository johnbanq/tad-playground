#ifndef TGI_BST_INTERNAL_BST_COMMON_H_
#define TGI_BST_INTERNAL_BST_COMMON_H_

#include<utility>
#include<string>
#include<vector>
#include<limits>

#include "tgi/bst/hex_util.h"

#include "tgi/bst/internal/internal_bst_literal.h"
#include "tgi/bst/internal/internal_bst_graphviz.h"
#include "tgi/bst/internal/internal_bst_violation.h"

// recursive delete //

template<typename node_type>
void delete_tree(node_type* n) {
    if(n!=nullptr) {
        delete_tree(n->left);
        delete_tree(n->right);
        delete n;
    }
}

// parent and ref locator //

template<typename node_type>
std::pair<node_type*, node_type**> locate_parent_and_expected_ref(node_type* parent, node_type** source_ref, int value) {
    node_type* node = *source_ref;
    if(node == nullptr) {
        return std::make_pair(parent, source_ref);
    } else {
        if(value < node->value) {
            return locate_parent_and_expected_ref(node, &(node->left), value);
        } else if(value == node->value) {
            return std::make_pair(parent, source_ref);
        } else {
            return locate_parent_and_expected_ref(node, &(node->right), value);
        }
    }
} 

template<typename tree_type>
using node_ptr_type_of = decltype(((tree_type*)nullptr)->root);

/**
 * this function, and its recursive case overload locates the parent and ref(pointer to pointer that should point to the node) of the node
 */
template<typename tree_type>
std::pair<node_ptr_type_of<tree_type>, node_ptr_type_of<tree_type>*> locate_parent_and_expected_ref(tree_type& tree, int value) {
    return locate_parent_and_expected_ref((node_ptr_type_of<tree_type>)nullptr, &(tree.root), value);
} 


#endif