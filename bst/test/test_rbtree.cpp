#include <vector>

#include "catch.hpp"

#include "test_bst_common.h"
#include "tgi/bst/rbtree.h"
#include "tgi/bst/internal/internal_rbtree.h"

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

TEST_CASE( "to_literal works on rbtree", "[rbtree][literal]" ) {
    REQUIRE(to_literal(rbtree_from_literal("(2B:(1R):(3R))")) == "(2B:(1R):(3R))");
}

TEST_CASE( "rbtree_node_stmt works", "[rbtree][visualize]" ) {
    auto tree = rbtree_from_literal("(2R)");
    REQUIRE(rbtree_node_stmt(tree.root) == "2[label=\"2("+addr_string(tree.root)+")\"][color=red];");
    
    tree = rbtree_from_literal("(2B)");
    REQUIRE(rbtree_node_stmt(tree.root) == "2[label=\"2("+addr_string(tree.root)+")\"][color=black];");
}

TEST_CASE( "to_graphviz works on rbtree", "[rbtree][visualize]" ) {
    auto tree = rbtree_from_literal("(2B:(1R):(3R))");
    REQUIRE(to_graphviz(tree) == "digraph bst{"
        + rbtree_node_stmt(tree.root)
        + edge_stmt(2, 1, "left")
        + edge_stmt(2, 3, "right")
        + rbtree_node_stmt(tree.root->left)
        + edge_stmt(1, 2, "parent")
        + rbtree_node_stmt(tree.root->right)
        + edge_stmt(3, 2, "parent")
        + "}"
    );
}

TEST_CASE( "find_violation for avl is also validate bst constraints", "[rbtree][verify]" ) {
    auto tree = rbtree_from_literal("(4B:(2B:(1B):(3B)):(6B:(5B):(7B)))");
    auto root = tree.root->left;
    root->left->parent = root->right;
    REQUIRE(find_violation(tree) == std::vector<std::string>{parent_violation(root, root->left)});

    tree = rbtree_from_literal("(2B:(0B):(1B))");
    REQUIRE(find_violation(tree) == std::vector<std::string>{"{1} is in wrong place: it should must be in range (2,int_max]"});
}

TEST_CASE( "find_violation will only check avl constraint if have no bst violations", "[rbtree][verify]" ) {
    auto tree = rbtree_from_literal("(2R:(1B):(3B))");
    tree.root->left->parent = tree.root->right;
    REQUIRE(find_violation(tree).size() == 1);
}

TEST_CASE( "find_violation rejects red root", "[rbtree][verify]" ) {
    auto tree = rbtree_from_literal("(2R)");
    REQUIRE(find_violation(tree) == std::vector<std::string>{"root must not be red"});
}