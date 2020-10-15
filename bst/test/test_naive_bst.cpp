#include "catch.hpp"

#include "test_bst_common.h"
#include "tgi/bst/naive_bst.h"
#include "tgi/bst/internal/internal_naive_bst.h"

// naive bst test //

// from_literal //

TEST_CASE( "from_literal builds empty tree on null", "[bst][literal]" ) {
    auto tree = bst_from_literal("null");
    REQUIRE(tree.root == nullptr);
}

TEST_CASE( "from_literal builds root tree", "[bst][literal]" ) {
    auto tree = bst_from_literal("(1)");
    REQUIRE(tree.root->value == 1);
    REQUIRE(tree.root->left == nullptr);
    REQUIRE(tree.root->right == nullptr);
}

TEST_CASE( "from_literal builds 1 child case", "[bst][literal]" ) {
    auto tree = bst_from_literal("(2:(1):null)");
    REQUIRE(tree.root->value == 2);
    REQUIRE(tree.root->left->parent == tree.root);
    REQUIRE(tree.root->left->value == 1);
    REQUIRE(tree.root->right == nullptr);

    tree = bst_from_literal("(2:null:(3))");
    REQUIRE(tree.root->value == 2);
    REQUIRE(tree.root->left == nullptr);
    REQUIRE(tree.root->right->parent == tree.root);
    REQUIRE(tree.root->right->value == 3);
}

TEST_CASE( "from_literal builds 2 child case", "[bst][literal]" ) {
    auto tree = bst_from_literal("(2:(1):(3))");
    REQUIRE(tree.root->value == 2);
    REQUIRE(tree.root->left->parent == tree.root);
    REQUIRE(tree.root->left->value == 1);
    REQUIRE(tree.root->right->parent == tree.root);
    REQUIRE(tree.root->right->value == 3);
}

TEST_CASE( "from_literal performs recursively", "[bst][literal]" ) {
    auto tree = bst_from_literal("(4:(2:(1):(3)):(6:(5):(7)))");
    REQUIRE(tree.root->value == 4);

    REQUIRE(tree.root->left->parent == tree.root);
    REQUIRE(tree.root->left->value == 2);
    REQUIRE(tree.root->left->left->parent == tree.root->left);
    REQUIRE(tree.root->left->left->value == 1);
    
    REQUIRE(tree.root->right->parent == tree.root);
    REQUIRE(tree.root->right->value == 6);
    REQUIRE(tree.root->right->right->parent == tree.root->right);
    REQUIRE(tree.root->right->right->value == 7);
}

// to_literal //

TEST_CASE( "to_literal prints null on empty tree", "[bst][literal]" ) {
    REQUIRE(to_literal(bst_from_literal("null")) == "null");
}

TEST_CASE( "to_literal handles 0 child case", "[bst][literal]" ) {
    REQUIRE(to_literal(bst_from_literal("(1)")) == "(1)");
}

TEST_CASE( "to_literal handles 1 child case", "[bst][literal]" ) {
    REQUIRE(to_literal(bst_from_literal("(2:(1):null)")) == "(2:(1):null)");
    REQUIRE(to_literal(bst_from_literal("(2:null:(3))")) == "(2:null:(3))");
}

TEST_CASE( "to_literal handles 2 child case", "[bst][literal]" ) {
    REQUIRE(to_literal(bst_from_literal("(2:(1):(3))")) == "(2:(1):(3))");
}

TEST_CASE( "to_literal performs recursively", "[bst][literal]" ) {
    REQUIRE(to_literal(bst_from_literal("(4:(2:(1):(3)):(6:(5):(7)))")) == "(4:(2:(1):(3)):(6:(5):(7)))");
}

// to_graphviz //

TEST_CASE( "node_stmt works", "[bst][visualize]" ) {
    auto tree = bst_from_literal("(2)");
    REQUIRE(node_stmt(2, tree.root) == "2[label=\"2("+addr_string(tree.root)+")\"];");
}

TEST_CASE( "edge_stmt works", "[bst][visualize]" ) {
    REQUIRE(edge_stmt(2, 1, "left") == "2 -> 1 [label=\"left\"];");
}

TEST_CASE( "edge_stmt gives grey on parent", "[bst][visualize]" ) {
    REQUIRE(edge_stmt(2, 1, "parent") == "2 -> 1 [label=\"parent\"][color=grey][fontcolor=grey];");
}

TEST_CASE( "to_graphviz handles null case", "[bst][visualize]" ) {
    REQUIRE(to_graphviz(bst_from_literal("null")) == "digraph bst{}");
}

TEST_CASE( "to_graphviz handles root only case", "[bst][visualize]" ) {
    auto tree = bst_from_literal("(2)");
    REQUIRE(to_graphviz(tree) == "digraph bst{"+node_stmt(2, tree.root)+"}");
}

TEST_CASE( "to_graphviz handles handles has 1 child case", "[bst][visualize]" ) {
    auto tree = bst_from_literal("(2:(1):null)");
    REQUIRE(to_graphviz(tree) == "digraph bst{"
        + node_stmt(2, tree.root)
        + edge_stmt(2, 1, "left")
        + node_stmt(1, tree.root->left)
        + edge_stmt(1, 2, "parent")
        + "}"
    );

    tree = bst_from_literal("(2:null:(3))");
    REQUIRE(to_graphviz(tree) == "digraph bst{"
        + node_stmt(2, tree.root)
        + edge_stmt(2, 3, "right")
        + node_stmt(3, tree.root->right)
        + edge_stmt(3, 2, "parent")
        + "}"
    );
}

TEST_CASE( "to_graphviz handles handles has 2 child case", "[bst][visualize]" ) {
    auto tree = bst_from_literal("(2:(1):(3))");
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

TEST_CASE( "to_graphviz performs recursively", "[bst][visualize]" ) {
    auto tree = bst_from_literal("(4:(2:(1):null):null)");
    REQUIRE(to_graphviz(tree) == "digraph bst{"
        + node_stmt(4, tree.root)
        + edge_stmt(4, 2, "left")
        + node_stmt(2, tree.root->left)
        + edge_stmt(2, 4, "parent")
        + edge_stmt(2, 1, "left")
        + node_stmt(1, tree.root->left->left)
        + edge_stmt(1, 2, "parent")
        + "}"
    );

    tree = bst_from_literal("(4:null:(6:null:(7)))");
    REQUIRE(to_graphviz(tree) == "digraph bst{"
        + node_stmt(4, tree.root)
        + edge_stmt(4, 6, "right")
        + node_stmt(6, tree.root->right)
        + edge_stmt(6, 4, "parent")
        + edge_stmt(6, 7, "right")
        + node_stmt(7, tree.root->right->right)
        + edge_stmt(7, 6, "parent")
        + "}"
    );
}

// verification test //

TEST_CASE( "parent_violation works", "[bst][verify]" ) {
 auto tree = bst_from_literal("(2:(1):(3))");
    auto parent = tree.root;
    auto child = tree.root->left;
    child->parent = parent->right;
    REQUIRE(parent_violation(parent, child) == "node {"+std::to_string(child->value)+"}'s parent should point to {"+addr_string(parent)+"}, not {"+addr_string(child->parent)+"}");
}


TEST_CASE( "find_violation accepts empty tree", "[bst][verify]" ) {
    auto tree = bst_from_literal("null");
    REQUIRE(find_violation(tree) == std::vector<std::string>{});
}

TEST_CASE( "find_violation works", "[bst][verify]" ) {
    auto tree = bst_from_literal("(2:(1):(3))");
    REQUIRE(find_violation(tree) == std::vector<std::string>{});
}

TEST_CASE( "find_violation rejects error in pointer", "[bst][verify]" ) {
    auto tree = bst_from_literal("(2:(1):null)");
    tree.root->parent = tree.root->left;
    REQUIRE(find_violation(tree) == std::vector<std::string>{"root's parent must be null!"});

    tree = bst_from_literal("(2:(1):(3))");
    tree.root->left->parent = tree.root->right;
    REQUIRE(find_violation(tree) == std::vector<std::string>{parent_violation(tree.root, tree.root->left)});

    tree = bst_from_literal("(2:(1):(3))");
    tree.root->right->parent = tree.root->left;
    REQUIRE(find_violation(tree) == std::vector<std::string>{parent_violation(tree.root, tree.root->right)});
}

TEST_CASE( "find_violation rejects error in pointer recursively", "[bst][verify]" ) {
    auto tree = bst_from_literal("(4:(2:(1):(3)):(6:(5):(7)))");
    auto root = tree.root->left;
    root->left->parent = root->right;
    REQUIRE(find_violation(tree) == std::vector<std::string>{parent_violation(root, root->left)});

    tree = bst_from_literal("(4:(2:(1):(3)):(6:(5):(7)))");
    root = tree.root->right;
    root->right->parent = root->left;
    REQUIRE(find_violation(tree) == std::vector<std::string>{parent_violation(root, root->right)});
}

TEST_CASE( "find_violation rejects error in value", "[bst][verify]" ) {
    auto tree = bst_from_literal("(2:(3):(4))");
    REQUIRE(find_violation(tree) == std::vector<std::string>{"{3} is in wrong place: it should must be in range (int_min,2]"});

    tree = bst_from_literal("(2:(0):(1))");
    REQUIRE(find_violation(tree) == std::vector<std::string>{"{1} is in wrong place: it should must be in range (2,int_max]"});
}

TEST_CASE( "find_violation rejects error in value recursively", "[bst][verify]" ) {
    auto tree = bst_from_literal("(4:(2:(1):(9)):(6:(5):(7)))");
    REQUIRE(find_violation(tree) == std::vector<std::string>{"{9} is in wrong place: it should must be in range (2,4]"});

    tree = bst_from_literal("(4:(2:(0):(3)):(6:(1):(7)))");
    REQUIRE(find_violation(tree) == std::vector<std::string>{"{1} is in wrong place: it should must be in range (4,6]"});
}

// assert utility //

// locate parent and ref test //

TEST_CASE("locate_parent_and_expected_ref locates root correctly", "[bst][locate]" ) {
    auto tree = bst_from_literal("(2)");
    REQUIRE(locate_parent_and_expected_ref(tree, 2) == std::make_pair((bst::node*)nullptr, &tree.root));
}

TEST_CASE("locate_parent_and_expected_ref locates node's pointer correctly", "[bst][locate]" ) {
    auto tree = bst_from_literal("(2:(1):(3))");
    REQUIRE(locate_parent_and_expected_ref(tree, 1) == std::make_pair(tree.root, &(tree.root->left)));
    REQUIRE(locate_parent_and_expected_ref(tree, 3) == std::make_pair(tree.root, &(tree.root->right)));

    tree = bst_from_literal("(2)");
    REQUIRE(locate_parent_and_expected_ref(tree, 1) == std::make_pair(tree.root, &(tree.root->left)));
    REQUIRE(locate_parent_and_expected_ref(tree, 3) == std::make_pair(tree.root, &(tree.root->right)));
}

// search test //

TEST_CASE("search works on empty tree", "[bst][search]" ) {
    auto tree = bst_from_literal("null");
    REQUIRE(search(tree, 2) == false);
}

TEST_CASE("search works on sample tree", "[bst][search]" ) {
    auto tree = bst_from_literal("(10:(4:(3):(5)):(15:(12):(17)))");
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


// insert test //

TEST_CASE( "insert ignores duplicate value", "[bst][insert]" ) {
    auto tree = bst_from_literal("(2)");
    insert(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2)");
}

TEST_CASE( "insert works on empty tree", "[bst][insert]" ) {
    auto tree = bst_from_literal("null");
    insert(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2)");
}

TEST_CASE( "insert works on sample tree", "[bst][insert]" ) {
    auto tree = bst_from_literal("(10:(4:(3):null):(15:null:(17)))");
    insert(tree, 5);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(10:(4:(3):(5)):(15:null:(17)))");

    insert(tree, 12);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(10:(4:(3):(5)):(15:(12):(17)))");
}

// remove test //

TEST_CASE( "remove accepts non-existent value", "[bst][remove]" ) {
    auto tree = bst_from_literal("(2:(1):(3))");
    remove(tree, 10);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2:(1):(3))");
}

TEST_CASE( "remove works on no child case", "[bst][remove]" ) {
    auto tree = bst_from_literal("(2:(1):(3))");
    remove(tree, 1);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2:null:(3))");

    remove(tree, 3);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2)");
}

TEST_CASE( "remove works on 1 child case", "[bst][remove]" ) {
    auto tree = bst_from_literal("(2:(1):null)");
    remove(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(1)");

    tree = bst_from_literal("(2:null:(3))");
    remove(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(3)");
}

TEST_CASE( "remove works on 2 child case", "[bst][remove]" ) {
    auto tree = bst_from_literal("(2:(1):(3))");
    remove(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(3:(1):null)");
}


