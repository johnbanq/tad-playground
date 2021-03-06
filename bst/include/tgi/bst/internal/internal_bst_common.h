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

// count element //

template<typename node_type>
unsigned int count_internal(node_type* root) {
    if(root != nullptr) {
        return 1 + count_internal(root->left) + count_internal(root->right); 
    } else {
        return 0;
    }
}

// list_all element //

template<typename node_type>
void list_all_internal(node_type* root, std::vector<int>& elems) {
    if(root!=nullptr) {
        list_all_internal(root->left, elems);
        elems.push_back(root->value);
        list_all_internal(root->right, elems);
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

template<typename tree_type, typename node_type>
node_type** ref_of(tree_type& tree, node_type* node) {
    if(node->parent == nullptr) {
        if(tree.root == node) {
            return &tree.root;
        } else {
            //impossible
            throw std::logic_error("invalid node: {"+std::to_string(node->value)+"} is not root and has no parent!");
        }
    } else {
        if(node->parent->left == node) {
            return &(node->parent->left);
        } else if(node->parent->right == node) {
            return &(node->parent->right);
        } else {
            //impossible
            throw std::logic_error("invalid node: {"+std::to_string(node->value)+"} has a parent {"+std::to_string(node->parent->value)+"} but is not any of its child!");
        }
    }
}


// left & right rotate //

template<typename node_type>
void left_rotate(node_type*& n) {
    auto node = n;
    auto left = node->left;
    auto right = node->right;
    auto rightleft = right->left;
    auto rightright = right->right;

    n = right;
    right->parent = node->parent;

    node->right = rightleft;
    if(rightleft != nullptr) {
        rightleft->parent = node;
    }

    right->left = node;
    node->parent = right;
}

template<typename node_type>
void right_rotate(node_type*& n) {
    auto node = n;
    auto left = node->left;
    auto right = node->right;
    auto leftleft = left->left;
    auto leftright = left->right;

    n = left;
    left->parent = node->parent;
    
    node->left = leftright;
    if(leftright != nullptr) {
        leftright->parent = node;
    }

    left->right = node;
    node->parent = left;
}

#endif