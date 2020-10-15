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

#endif