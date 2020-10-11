#ifndef TGI_BST_NAIVE_BST_H_
#define TGI_BST_NAIVE_BST_H_

#include<string>
#include<stdexcept>
#include<vector>

struct bst {

    struct node {
        int value;
        node* parent;
        node* left;
        node* right;
    };

    // object semantics, rejects copy

    bst(node* root) 
        :root(root){}

    bst(const bst&) = delete;
    bst& operator=(bst&) = delete;
    bst(bst&&);
    bst& operator=(bst&&);
    
    ~bst();

    node* root;

};

/**
 * \brief generates a bst from literal:
 *  "null“ should give a empty tree,
 *  "(1)" should give a tree with only 1 root node with value of 1
 *  "(2:(1):null)" should give a tree with root of 2 and left child of 1 and have no right child
 *  "(2:(1):(3))" should give a tree with root of 2 and left child of 1 and right child of 3
 * 
 *  nodeLiteral = "null" | (number) | (number:nodeLiteral:nodeLiteral)
 * 
 * \param literal the literal string
 * \throw std::invalid_argument if the literal is invalid
 * \return bst the tree
 */
bst from_literal(const std::string& literal);

/**
 * \brief construct the literal from the tree
 * 
 * \param tree the tree
 * \return std::string the constructed literal
 */
std::string to_literal(const bst& tree);

/**
 * \brief construct the graphviz code that represents the tree
 * 
 * \param tree the tree
 * \return std::string the graphviz code
 */
std::string to_graphviz(const bst& tree);

/**
 * \brief check the validity of the binary search tree
 * 
 * \param tree the tree to check
 * \return std::vector<std::string> the detected violations, empty if none 
 */
std::vector<std::string> find_violation(const bst& tree);

/**
 * \brief insert an element into the tree
 * 
 * \param tree the tree
 * \param value the value to insert
 */
void insert(bst& tree, int value);

#endif