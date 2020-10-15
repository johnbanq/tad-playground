#ifndef TGI_BST_INTERNAL_AVL_H_
#define TGI_BST_INTERNAL_AVL_H_

#include "tgi/bst/avl.h"
#include "tgi/bst/hex_util.h"
#include "tgi/bst/internal/internal_bst_common.h"

// others //

static std::string avl_node_stmt(const avl::node* n) {
    return std::to_string(n->value)+"[label=\""+std::to_string(n->value)+"(h:"+std::to_string(n->height)+")"+"(a:"+addr_string(n)+")\"];";
}

void left_rotate(avl::node*& n);

void right_rotate(avl::node*& n);

void rebalance(avl::node*& n);


#endif