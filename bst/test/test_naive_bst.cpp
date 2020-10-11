#include <vector>

#include "catch.hpp"

#include "tgi/bst/naive_bst.h"
#include "tgi/bst/internal_naive_bst.h"

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
    REQUIRE(tree.root->left->parent == tree.root);
    REQUIRE(tree.root->left->value == 1);
    REQUIRE(tree.root->right == nullptr);

    tree = from_literal("(2:null:(3))");
    REQUIRE(tree.root->value == 2);
    REQUIRE(tree.root->left == nullptr);
    REQUIRE(tree.root->right->parent == tree.root);
    REQUIRE(tree.root->right->value == 3);
}

TEST_CASE( "from_literal builds 2 child case", "[bst][literal]" ) {
    auto tree = from_literal("(2:(1):(3))");
    REQUIRE(tree.root->value == 2);
    REQUIRE(tree.root->left->parent == tree.root);
    REQUIRE(tree.root->left->value == 1);
    REQUIRE(tree.root->right->parent == tree.root);
    REQUIRE(tree.root->right->value == 3);
}


TEST_CASE( "to_literal prints null on empty tree", "[bst][literal]" ) {
    REQUIRE(to_literal(from_literal("null")) == "null");
}

TEST_CASE( "to_literal handles 0 child case", "[bst][literal]" ) {
    REQUIRE(to_literal(from_literal("(1)")) == "(1)");
}

TEST_CASE( "to_literal handles 1 child case", "[bst][literal]" ) {
    REQUIRE(to_literal(from_literal("(2:(1):null)")) == "(2:(1):null)");
    REQUIRE(to_literal(from_literal("(2:null:(3))")) == "(2:null:(3))");
}

TEST_CASE( "to_literal handles 2 child case", "[bst][literal]" ) {
    REQUIRE(to_literal(from_literal("(2:(1):(3))")) == "(2:(1):(3))");
}


TEST_CASE( "node_stmt works", "[bst][visualize]" ) {
    auto tree = from_literal("(2)");
    REQUIRE(node_stmt(2, tree.root) == "2[label=\"2("+int_to_hex((uint16_t(tree.root)))+")\"];");
}

TEST_CASE( "edge_stmt works", "[bst][visualize]" ) {
    REQUIRE(edge_stmt(2, 1, "left") == "2 -> 1 [label=\"left\"];");
}

TEST_CASE( "edge_stmt gives grey on parent", "[bst][visualize]" ) {
    REQUIRE(edge_stmt(2, 1, "parent") == "2 -> 1 [label=\"parent\"][color=grey][fontcolor=grey];");
}

TEST_CASE( "to_graphviz handles null case", "[bst][visualize]" ) {
    REQUIRE(to_graphviz(from_literal("null")) == "digraph bst{}");
}

TEST_CASE( "to_graphviz handles root only case", "[bst][visualize]" ) {
    auto tree = from_literal("(2)");
    REQUIRE(to_graphviz(tree) == "digraph bst{"+node_stmt(2, tree.root)+"}");
}

TEST_CASE( "to_graphviz handles handles has 1 child case", "[bst][visualize]" ) {
    auto tree = from_literal("(2:(1):null)");
    REQUIRE(to_graphviz(tree) == "digraph bst{"
        + node_stmt(2, tree.root)
        + edge_stmt(2, 1, "left")
        + node_stmt(1, tree.root->left)
        + edge_stmt(1, 2, "parent")
        + "}"
    );

    tree = from_literal("(2:null:(3))");
    REQUIRE(to_graphviz(tree) == "digraph bst{"
        + node_stmt(2, tree.root)
        + edge_stmt(2, 3, "right")
        + node_stmt(3, tree.root->right)
        + edge_stmt(3, 2, "parent")
        + "}"
    );
}

TEST_CASE( "to_graphviz handles handles has 2 child case", "[bst][visualize]" ) {
    auto tree = from_literal("(2:(1):(3))");
    REQUIRE(to_graphviz(tree) == "digraph bst{"
        + node_stmt(2, tree.root)
        + edge_stmt(2, 1, "left")
        + edge_stmt(2, 3, "right")
        + node_stmt(1, tree.root->left)
        + edge_stmt(1, 2, "parent")
        + node_stmt(3, tree.root->right)
        + edge_stmt(3, 2, "parent")
        + "}"
    );
}