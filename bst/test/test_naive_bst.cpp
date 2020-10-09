#include <vector>

#include "catch.hpp"

#include "tgi/bst/naive_bst.h"

// naive bst test //

TEST_CASE( "from_literal builds empty tree on null", "[bst][literal]" ) {
    auto tree = from_literal("null");
    REQUIRE(tree.root == nullptr);
}

TEST_CASE( "from_literal builds root tree", "[bst][literal]" ) {
    auto tree = from_literal("(1)");
    REQUIRE(tree.root->value == 1);
    REQUIRE(tree.root->left == nullptr);
    REQUIRE(tree.root->right == nullptr);
}

TEST_CASE( "from_literal builds 1 child case", "[bst][literal]" ) {
    auto tree = from_literal("(2:(1):null)");
    REQUIRE(tree.root->value == 2);
    REQUIRE(tree.root->left->value == 1);
    REQUIRE(tree.root->right == nullptr);

    tree = from_literal("(2:null:(3))");
    REQUIRE(tree.root->value == 2);
    REQUIRE(tree.root->left == nullptr);
    REQUIRE(tree.root->right->value == 3);
}