#include <vector>
#include <set>

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

TEST_CASE( "find_violation will only check rbtree constraint if have no bst violations", "[rbtree][verify]" ) {
    auto tree = rbtree_from_literal("(2R:(1B):(3B))");
    tree.root->left->parent = tree.root->right;
    REQUIRE(find_violation(tree).size() == 1);
}

TEST_CASE( "find_violation rejects red root", "[rbtree][verify]" ) {
    auto tree = rbtree_from_literal("(2R)");
    REQUIRE(find_violation(tree) == std::vector<std::string>{"root must not be red"});
}

TEST_CASE( "find_violation rejects red child of red node", "[rbtree][verify]" ) {
    auto tree = rbtree_from_literal("(4B:(2R:(1R):(3R)):null)");
    REQUIRE(find_violation(tree) == std::vector<std::string>{"red node {2}'s child {1} cannot be a red node!", "red node {2}'s child {3} cannot be a red node!"});
}


TEST_CASE( "enumerate_node_to_leaf_path enumerate all path from node to leaf", "[rbtree][verify]" ) {
    auto tree = rbtree_from_literal("(4B:(2B:(1B):(3B)):(5B))");
    std::vector<std::vector<const rbtree::node*>> expected_paths {
        //left nullptr and right nullptr are not considered as same, hence double
        std::vector<const rbtree::node*>{tree.root, tree.root->left, tree.root->left->left, nullptr},
        std::vector<const rbtree::node*>{tree.root, tree.root->left, tree.root->left->left, nullptr},
        std::vector<const rbtree::node*>{tree.root, tree.root->left, tree.root->left->right, nullptr},
        std::vector<const rbtree::node*>{tree.root, tree.root->left, tree.root->left->right, nullptr},
        std::vector<const rbtree::node*>{tree.root, tree.root->right, nullptr},
        std::vector<const rbtree::node*>{tree.root, tree.root->right, nullptr}
    };
    std::vector<std::vector<const rbtree::node*>> actual_paths;

    enumerate_node_to_leaf_path(tree.root, [&](const std::vector<const rbtree::node*>& path){actual_paths.push_back(path);});

    REQUIRE(actual_paths == expected_paths);
}

TEST_CASE( "enumerate_preorder enumerate node correctly", "[rbtree][verify]" ) {
    auto tree = rbtree_from_literal("(4B:(2B:(1B):(3B)):(5B))");
    std::vector<const rbtree::node*> expected_nodes{
        tree.root, tree.root->left, tree.root->left->left, tree.root->left->right, tree.root->right
    };
    std::vector<const rbtree::node*> actual_nodes;

    enumerate_node_preorder(tree.root, [&](const rbtree::node* node){actual_nodes.push_back(node);});

    REQUIRE(actual_nodes == expected_nodes);
}

TEST_CASE( "find_violation rejects unequal black node count", "[rbtree][verify]" ) {
    auto tree = rbtree_from_literal("(4B:null:(6B))");
    REQUIRE(find_violation(tree) == std::vector<std::string>{
        "node{4}'s path {4B}->{6B}->nullptr has a black node count of 2, should be 1",
        "node{4}'s path {4B}->{6B}->nullptr has a black node count of 2, should be 1"
    });
}

// rotate and rabalancing //

TEST_CASE("left_rotate works", "[rbtree][rotate]" ) {
    auto tree = rbtree_from_literal("(2B:(1B):(3B))");
    rbtree_left_rotate(tree.root);
    REQUIRE(to_literal(tree) == "(3B:(2B:(1B):null):null)");
}

TEST_CASE("right_rotate works", "[rbtree][rotate]" ) {
    auto tree = rbtree_from_literal("(2B:(1B):(3B))");
    rbtree_right_rotate(tree.root);
    REQUIRE(to_literal(tree) == "(1B:null:(2B:null:(3B)))");
}

// insert //

TEST_CASE("perform_insert_fixup is no-op on black parent", "[rbtree][insert]" ) {
    auto tree = rbtree_from_literal("(2B:(1R):null)");
    REQUIRE(perform_insert_fixup(tree, tree.root->left) == nullptr);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2B:(1R):null)");
}

TEST_CASE("perform_insert_fixup handles root fixup", "[rbtree][insert]" ) {
    auto tree = rbtree_from_literal("(2R)");
    REQUIRE(perform_insert_fixup(tree, tree.root) == nullptr);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2B)");
}

TEST_CASE("perform_insert_fixup handles left case 1", "[rbtree][insert]" ) {
    auto tree = rbtree_from_literal("(6B:(4R:(3R):null):(7R))");
    REQUIRE(perform_insert_fixup(tree, tree.root->left->left) == tree.root);
    REQUIRE(find_violation(tree) == std::vector<std::string>{"root must not be red"});
    REQUIRE(to_literal(tree) == "(6R:(4B:(3R):null):(7B))");

    tree = rbtree_from_literal("(6B:(4R:null:(5R)):(7R))");
    REQUIRE(perform_insert_fixup(tree, tree.root->left->right) == tree.root);
    REQUIRE(find_violation(tree) == std::vector<std::string>{"root must not be red"});
    REQUIRE(to_literal(tree) == "(6R:(4B:null:(5R)):(7B))");
}

TEST_CASE("perform_insert_fixup handles left case 3", "[rbtree][insert]" ) {
    auto tree = rbtree_from_literal("(6B:(4R:(3R):null):null)");
    REQUIRE(perform_insert_fixup(tree, tree.root->left->left) == nullptr);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(4B:(3R):(6R))");
}

TEST_CASE("perform_insert_fixup handles left case 2", "[rbtree][insert]" ) {
    auto tree = rbtree_from_literal("(6B:(4R:null:(5R)):null)");
    REQUIRE(perform_insert_fixup(tree, tree.root->left->right) == nullptr);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(5B:(4R):(6R))");
}

TEST_CASE("perform_insert_fixup handles right case 1", "[rbtree][insert]" ) {
    auto tree = rbtree_from_literal("(6B:(3R):(8R:(7R):null))");
    REQUIRE(perform_insert_fixup(tree, tree.root->right->left) == tree.root);
    REQUIRE(find_violation(tree) == std::vector<std::string>{"root must not be red"});
    REQUIRE(to_literal(tree) == "(6R:(3B):(8B:(7R):null))");

    tree = rbtree_from_literal("(6B:(3R):(8R:null:(10R)))");
    REQUIRE(perform_insert_fixup(tree, tree.root->right->right) == tree.root);
    REQUIRE(find_violation(tree) == std::vector<std::string>{"root must not be red"});
    REQUIRE(to_literal(tree) == "(6R:(3B):(8B:null:(10R)))");
}

TEST_CASE("perform_insert_fixup handles right case 2", "[rbtree][insert]" ) {
    auto tree = rbtree_from_literal("(6B:null:(10R:(9R):null))");
    REQUIRE(perform_insert_fixup(tree, tree.root->right->left) == nullptr);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(9B:(6R):(10R))");
}

TEST_CASE("perform_insert_fixup handles right case 3", "[rbtree][insert]" ) {
    auto tree = rbtree_from_literal("(6B:null:(7R:null:(8R)))");
    REQUIRE(perform_insert_fixup(tree, tree.root->right->right) == nullptr);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(7B:(6R):(8R))");
}

TEST_CASE("rbtree insert works", "[rbtree][insert]" ) {
    auto tree = rbtree_from_literal("null");
    insert(tree, 2);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2B)");

    insert(tree, 1);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2B:(1R):null)");

    insert(tree, 3);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(2B:(1R):(3R))");
}

TEST_CASE("rbtree insert performs fix up to the tree", "[rbtree][insert]" ) {
    auto tree = rbtree_from_literal("(6B:(3R):(8R))");
    insert(tree, 7);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(6B:(3B):(8B:(7R):null))");
}


TEST_CASE("perform_delete_fixup handles handles root case", "[rbtree][delete]" ) {
    auto tree = rbtree_from_literal("(6R)");
    REQUIRE(perform_delete_fixup(tree, nullptr, tree.root) == nullptr);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(6B)");

    tree = rbtree_from_literal("(6B)");
    REQUIRE(perform_delete_fixup(tree, nullptr, tree.root) == nullptr);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(6B)");
}

TEST_CASE("perform_delete_fixup handles handles red node case", "[rbtree][delete]" ) {
    auto tree = rbtree_from_literal("(6B:(3R):(7B))");
    REQUIRE(perform_delete_fixup(tree, tree.root, tree.root->left) == nullptr);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(6B:(3B):(7B))");
}

TEST_CASE("perform_delete_fixup handles handles left case 1", "[rbtree][delete]" ) {
    //note: this example is synthetic, does not represent a real possibility
    auto tree = rbtree_from_literal("(4B:(3B):(10R:(6B:(5B):(7B)):(13B:(11B):(15B))))");
    REQUIRE(perform_delete_fixup(tree, tree.root, tree.root->left) == tree.root->left);
    REQUIRE(to_literal(tree) == "(10B:(4R:(3B):(6R:(5B):(7B))):(13B:(11B):(15B)))");
    //this is just to make the tree valid, 4 is a red node, will just terminate
    REQUIRE(perform_delete_fixup(tree, tree.root, tree.root->left) == nullptr);
    REQUIRE(is_valid_tree(tree));
}

TEST_CASE("perform_delete_fixup handles handles left case 2", "[rbtree][delete]" ) {
    //null count as a black leaf, and we assume we removed (3B) from the tree, causing the duo black on the root->left "node"
    auto tree = rbtree_from_literal("(6B:null:(7B))");
    REQUIRE(perform_delete_fixup(tree, tree.root, tree.root->left) == tree.root);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(6B:null:(7R))");
}

TEST_CASE("perform_delete_fixup handles handles left case 3", "[rbtree][delete]" ) {
    //note: this example is synthetic, does not represent a real possibility
    auto tree = rbtree_from_literal("(6R:(3B):(10B:(8R:(7B):(9B)):(11B)))");
    REQUIRE(perform_delete_fixup(tree, tree.root, tree.root->left) == nullptr);
    REQUIRE(find_violation(tree) == std::vector<std::string>{"root must not be red"});
    REQUIRE(to_literal(tree) == "(8R:(6B:(3B):(7B)):(10B:(9B):(11B)))");
}

TEST_CASE("perform_delete_fixup handles handles left case 4", "[rbtree][delete]" ) {
    //note: this example is synthetic, does not represent a real possibility
    auto tree = rbtree_from_literal("(5R:(3B):(7B:(6B):(11R:(8B):(12B))))");
    REQUIRE(perform_delete_fixup(tree, tree.root, tree.root->left) == nullptr);
    REQUIRE(find_violation(tree) == std::vector<std::string>{"root must not be red"});
    REQUIRE(to_literal(tree) == "(7R:(5B:(3B):(6B)):(11B:(8B):(12B)))");
}

TEST_CASE("perform_delete_fixup handles handles right case 2", "[rbtree][delete]" ) {
    //null count as a black leaf, and we assume we removed (7B) from the tree, causing the duo black on the root->left "node"
    auto tree = rbtree_from_literal("(6B:(5B):null)");
    REQUIRE(perform_delete_fixup(tree, tree.root, tree.root->right) == tree.root);
    REQUIRE(is_valid_tree(tree));
    REQUIRE(to_literal(tree) == "(6B:(5R):null)");
}
