#include <vector>

#include "catch.hpp"

#include "test_bst_common.h"
#include "tgi/bst/avl.h"
#include "tgi/bst/internal_avl.h"


TEST_CASE( "from_literal works on avl", "[avl][literal]" ) {
    auto tree = avl_from_literal("(2:(1):(3))");
    REQUIRE(tree.root->value == 2);
    REQUIRE(tree.root->left->parent == tree.root);
    REQUIRE(tree.root->left->value == 1);
    REQUIRE(tree.root->right->parent == tree.root);
    REQUIRE(tree.root->right->value == 3);
}

TEST_CASE( "to_literal works on avl", "[avl][literal]" ) {
    REQUIRE(to_literal(avl_from_literal("(4:(2:(1):(3)):(6:(5):(7)))")) == "(4:(2:(1):(3)):(6:(5):(7)))");
}

TEST_CASE( "avl_node_stmt works", "[avl][visualize]" ) {
    auto tree = avl_from_literal("(2)");
    REQUIRE(avl_node_stmt(tree.root) == "2[label=\"2(h:0)(a:"+int_to_hex((uint16_t(tree.root)))+")\"];");
}

TEST_CASE( "to_graphviz works on avl", "[avl][visualize]" ) {
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


TEST_CASE( "find_violation for avl is also validate bst constraints", "[avl][verify]" ) {
    auto tree = avl_from_literal("(4:(2:(1):(3)):(6:(5):(7)))");
    auto root = tree.root->left;
    root->left->parent = root->right;
    REQUIRE(find_violation(tree) == std::vector<std::string>{parent_violation(root, root->left)});

    tree = avl_from_literal("(2:(0):(1))");
    REQUIRE(find_violation(tree) == std::vector<std::string>{"{1} is in wrong place: it should must be in range (2,int_max]"});
}

TEST_CASE( "find_violation will only check avl constraint if have no bst violations", "[avl][verify]" ) {
    auto tree = avl_from_literal("(2:(1):(3))");
    tree.root->left->parent = tree.root->right;
    tree.root->left->height = 1;
    REQUIRE(find_violation(tree).size() == 1);
}

TEST_CASE( "find_violation rejects invalid height", "[avl][verify]" ) {
    auto tree = avl_from_literal("(2:(1):(3))");
    tree.root->left->height = 1;
    REQUIRE(find_violation(tree) == std::vector<std::string>{"{1} has wrong height=1, should be height=0"});

    tree = avl_from_literal("(2:(1):(3))");
    tree.root->height = 2;
    REQUIRE(find_violation(tree) == std::vector<std::string>{"{2} has wrong height=2, should be height=1"});
}

TEST_CASE( "find_violation rejects inbalance", "[avl][verify]" ) {
    auto tree = avl_from_literal("(3:(2:(1):null):null)");
    REQUIRE(find_violation(tree) == std::vector<std::string>{"{3} has a bad balance factor of 2"});

    tree = avl_from_literal("(3:null:(4:null:(5)))");
    REQUIRE(find_violation(tree) == std::vector<std::string>{"{3} has a bad balance factor of -2"});
}


TEST_CASE("search works on sample avl", "[avl][search]" ) {
    auto tree = avl_from_literal("(10:(4:(3):(5)):(15:(12):(17)))");
    for (auto i : {3,4,5,10,12,15,17}) {
        REQUIRE(search(tree, i) == true);
    }

    //random samples

    //right-left & left-right leaf
    REQUIRE(search(tree, 6) == false);
    REQUIRE(search(tree, 13) == false);
    //intermediate stop
    REQUIRE(search(tree, 20) == false);
}

// rotate and rabalancing

TEST_CASE("left_rotate works", "[avl][rotate]" ) {
    auto tree = avl_from_literal("(4:(2:null:(3)):(5))");
    left_rotate(tree.root->left);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(4:(3:(2):null):(5))");
}

TEST_CASE("left_rotate works on root", "[avl][rotate]" ) {
    auto tree = avl_from_literal("(2:null:(3))");
    left_rotate(tree.root);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(3:(2):null)");
}

TEST_CASE("right_rotate works", "[avl][rotate]" ) {
    auto tree = avl_from_literal("(4:(2:(1):null):(5))");
    right_rotate(tree.root->left);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(4:(1:null:(2)):(5))");
}

TEST_CASE("right_rotate works on root", "[avl][rotate]" ) {
    auto tree = avl_from_literal("(2:(1):null)");
    right_rotate(tree.root);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(1:null:(2))");
}

TEST_CASE("rebalance is no-op on correct node", "[avl][rebalance]" ) {
    auto tree = avl_from_literal("(2:(1):(3))");
    rebalance(tree.root);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2:(1):(3))");

    tree = avl_from_literal("(2:null:(3))");
    rebalance(tree.root);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2:null:(3))");

    tree = avl_from_literal("(2:(1):null)");
    rebalance(tree.root);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2:(1):null)");
}

TEST_CASE("rebalance rebalances left-left case", "[avl][rebalance]" ) {
    auto tree = avl_from_literal("(3:(2:(1):null):null)");
    rebalance(tree.root);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2:(1):(3))");
}

TEST_CASE("rebalance rebalances left-right case", "[avl][rebalance]" ) {
    auto tree = avl_from_literal("(4:(2:null:(3)):null)");
    rebalance(tree.root);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(3:(2):(4))");
}

TEST_CASE("rebalance rebalances right-right case", "[avl][rebalance]" ) {
    auto tree = avl_from_literal("(4:null:(5:null:(6)))");
    rebalance(tree.root);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(5:(4):(6))");
}

TEST_CASE("rebalance rebalances right-left case", "[avl][rebalance]" ) {
    auto tree = avl_from_literal("(4:null:(6:(5):null))");
    rebalance(tree.root);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(5:(4):(6))");
}

// insert test //

TEST_CASE( "insert ignores existing value", "[avl][insert]" ) {
    auto tree = avl_from_literal("(2)");
    insert(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2)");
}

TEST_CASE( "insert works on empty tree", "[avl][insert]" ) {
    auto tree = avl_from_literal("null");
    insert(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2)");
}

TEST_CASE( "insert rebalances", "[avl][insert]" ) {
    auto tree = avl_from_literal("(10:(5):null)");
    insert(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(5:(2):(10))");
}

TEST_CASE( "insert rebalances to root", "[avl][insert]" ) {
    auto tree = avl_from_literal("(6:(4:(3):(5)):(10))");
    insert(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(4:(3:(2):null):(6:(5):(10)))");
}

// remove test //

TEST_CASE( "remove accepts non-existent value", "[avl][remove]" ) {
    auto tree = avl_from_literal("(2:(1):(3))");
    remove(tree, 10);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2:(1):(3))");
}

TEST_CASE( "remove works on no child case", "[avl][remove]" ) {
    auto tree = avl_from_literal("(2:(1):(3))");
    remove(tree, 1);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2:null:(3))");

    remove(tree, 3);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2)");
}

TEST_CASE( "remove works on 1 child case", "[avl][remove]" ) {
    auto tree = avl_from_literal("(2:(1):null)");
    remove(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(1)");

    tree = avl_from_literal("(2:null:(3))");
    remove(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(3)");
}

TEST_CASE( "remove works on 2 child case", "[avl][remove]" ) {
    auto tree = avl_from_literal("(2:(1):(3))");
    remove(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(3:(1):null)");
}

TEST_CASE( "remove rebalances", "[avl][remove]" ) {
    auto tree = avl_from_literal("(5:(4:(3):null):(10))");
    remove(tree, 10);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(4:(3):(5))");
}

TEST_CASE( "remove rebalances to root", "[avl][remove]" ) {
    auto tree = avl_from_literal("(6:(4:(2):null):(10:(9):(12:null:(13))))");
    remove(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(10:(6:(4):(9)):(12:null:(13)))");
}