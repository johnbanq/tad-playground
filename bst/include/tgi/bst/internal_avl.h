#ifndef TGI_BST_INTERNAL_AVL_H_
#define TGI_BST_INTERNAL_AVL_H_

#include "tgi/bst/avl.h"
#include "tgi/bst/hex_util.h"
#include "tgi/bst/internal_bst_common.h"

// others //

static std::string avl_node_stmt(const avl::node* n) {
    return std::to_string(n->value)+"[label=\""+std::to_string(n->value)+"(h:"+std::to_string(n->height)+")"+"(a:"+int_to_hex((uint16_t(n)))+")\"];";
}

#endif