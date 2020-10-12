#ifndef TGI_BST_INTERNAL_NAIVE_BST_H_
#define TGI_BST_INTERNAL_NAIVE_BST_H_

#include<utility>

#include "tgi/bst/hex_util.h"
#include "tgi/bst/internal_bst_common.h"
#include "tgi/bst/naive_bst.h"

// others //

/**
 * locate a pointer that points to the slot which should hold a pointer to the expected node(ref)
 * and the expected parent of the expected node
 */
std::pair<bst::node*, bst::node**> locate_parent_and_expected_ref(bst& tree, int value);

#endif