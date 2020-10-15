#ifndef TGI_BST_INTERNAL_BST_VIOLATION_H_
#define TGI_BST_INTERNAL_BST_VIOLATION_H_

#include<string>
#include<vector>
#include<limits>

#include "tgi/bst/hex_util.h"

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

template<typename tree_type>
void find_pointer_violation(const tree_type& tree, std::vector<std::string>& violations) {
    auto root = tree.root;
    if(root != nullptr && root->parent != nullptr) {
        violations.push_back("root's parent must be null!");
    }
    find_pointer_violation_on_node(root, violations);
}

template<typename node_type>
static std::string parent_violation(const node_type* parent, const node_type* child) {
    return "node {"+std::to_string(child->value)+"}'s parent should point to {"+addr_string(parent)+"}, not {"+addr_string(child->parent)+"}";
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

template<typename tree_type>
void find_value_violation(const tree_type& tree, std::vector<std::string>& violations) {
    return find_value_violation_on_node(tree.root, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), violations);
}

#endif