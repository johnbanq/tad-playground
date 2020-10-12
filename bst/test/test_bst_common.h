#ifndef TGI_BST_TEST_BST_COMMON_H_
#define TGI_BST_TEST_BST_COMMON_H_

#include <vector>
#include <string>

template<typename tree_type>
bool is_valid_tree(const tree_type& tree) {
    REQUIRE(find_violation(tree) == std::vector<std::string>{});
    //so we can write REQUIRE(is_valid_bst(tree));
    return true;
}

#endif