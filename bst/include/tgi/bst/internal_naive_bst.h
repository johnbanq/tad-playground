#ifndef TGI_BST_INTERNAL_NAIVE_BST_H_
#define TGI_BST_INTERNAL_NAIVE_BST_H_

#include "tgi/bst/hex_util.h"
#include "tgi/bst/naive_bst.h"

/**
 * construct the node statement of graphviz
 */
static std::string node_stmt(int value, const bst::node* addr) {
    return std::to_string(value)+"[label=\""+std::to_string(value)+"("+int_to_hex((uint16_t(addr)))+")\"];";
}

#endif