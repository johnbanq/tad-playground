#include <vector>

#include "catch.hpp"

#include "test_bst_common.h"
#include "tgi/bst/rbtree.h"
#include "tgi/bst/internal_rbtree.h"

using color = rbtree::color;

TEST_CASE( "from_literal works on rbtree", "[rbtree][literal]" ) {
    auto tree = rbtree_from_literal("(2B:(1R):(3R))");
    REQUIRE(tree.root->value == 2);
    REQUIRE(tree.root->color == color::black);
    REQUIRE(tree.root->left->parent == tree.root);
    REQUIRE(tree.root->left->value == 1);
    REQUIRE(tree.root->left->color == color::red);
    REQUIRE(tree.root->right->parent == tree.root);
    REQUIRE(tree.root->right->value == 3);
    REQUIRE(tree.root->right->color == color::red);
}