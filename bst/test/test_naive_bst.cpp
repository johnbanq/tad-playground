#include <vector>

#include "catch.hpp"

#include "tgi/bst/naive_bst.h"

// naive bst test //

TEST_CASE( "from_literal builds empty tree on null", "[bst][literal]" ) {
    auto tree = from_literal("null");
    REQUIRE(tree.root == nullptr);
}