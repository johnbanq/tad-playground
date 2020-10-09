#ifndef TGI_BST_NAIVE_BST_H_
#define TGI_BST_NAIVE_BST_H_

#include<string>

struct bst {

    struct node {
        int value;
        node* left;
        node* right;
    };

    node* root;

};

/**
 * \brief generates a bst from literal:
 *  "null“ should give a empty tree,
 *  "(1)" should give a tree with only 1 root node with value of 1
 *  "(2:(1):(3))" should give a tree with root of 2 and left child of 1 and right child of 3
 * 
 * \param literal the literal string
 * \return bst the tree
 */
bst from_literal(const std::string& literal);

#endif