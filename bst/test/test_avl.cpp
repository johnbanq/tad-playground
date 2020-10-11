#include <vector>

#include "catch.hpp"

#include "tgi/bst/avl.h"
#include "tgi/bst/internal_avl.h"
#include "tgi/bst/internal_naive_bst.h"


TEST_CASE( "from_literal works", "[avl][literal]" ) {
    auto tree = avl_from_literal("(2:(1):(3))");
    REQUIRE(tree.root->value == 2);
    REQUIRE(tree.root->left->parent == tree.root);
    REQUIRE(tree.root->left->value == 1);
    REQUIRE(tree.root->right->parent == tree.root);
    REQUIRE(tree.root->right->value == 3);
}

TEST_CASE( "to_literal works", "[avl][literal]" ) {
    REQUIRE(to_literal(avl_from_literal("(4:(2:(1):(3)):(6:(5):(7)))")) == "(4:(2:(1):(3)):(6:(5):(7)))");
}

TEST_CASE( "avl_node_stmt works", "[bst][visualize]" ) {
    auto tree = avl_from_literal("(2)");
    REQUIRE(avl_node_stmt(tree.root) == "2[label=\"2(h:0)(a:"+int_to_hex((uint16_t(tree.root)))+")\"];");
}

TEST_CASE( "to_graphviz works", "[avl][visualize]" ) {
    auto tree = avl_from_literal("(2:(1):(3))");
    REQUIRE(to_graphviz(tree) == "digraph bst{"
        + avl_node_stmt(tree.root)
        + edge_stmt(2, 1, "left")
        + edge_stmt(2, 3, "right")
        + avl_node_stmt(tree.root->left)
        + edge_stmt(1, 2, "parent")
        + avl_node_stmt(tree.root->right)
        + edge_stmt(3, 2, "parent")
        + "}"
    );
}