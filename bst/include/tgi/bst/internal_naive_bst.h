#ifndef TGI_BST_INTERNAL_NAIVE_BST_H_
#define TGI_BST_INTERNAL_NAIVE_BST_H_

#include<utility>

#include "tgi/bst/hex_util.h"
#include "tgi/bst/naive_bst.h"

/**
 * construct the node statement of graphviz
 */
static std::string node_stmt(int value, const bst::node* addr) {
    return std::to_string(value)+"[label=\""+std::to_string(value)+"("+int_to_hex((uint16_t(addr)))+")\"];";
}

/**
 * construct the edge statement of graphviz
 */
static std::string edge_stmt(int from, int to, const std::string& edge) {
    auto stmt = std::to_string(from)+" -> "+std::to_string(to)+" [label=\""+edge+"\"]";
    if(edge == "parent") {
        stmt += "[color=grey][fontcolor=grey]";
    }
    stmt += ";";
    return stmt;
}


static std::string parent_violation(const bst::node* parent, const bst::node* child) {
    return "node {"+std::to_string(child->value)+"}'s parent should point to {"+int_to_hex((uint16_t)parent)+"}, not {"+int_to_hex((uint16_t)child->parent)+"}";
}

/**
 * locate a pointer that points to the slot which should hold a pointer to the expected node(ref)
 * and the expected parent of the expected node
 */
std::pair<bst::node*, bst::node**> locate_parent_and_expected_ref(bst& tree, int value);

#endif