#ifndef TGI_BST_AVL_H_
#define TGI_BST_AVL_H_

#include<string>
#include<stdexcept>
#include<vector>

struct avl {

    struct node {
        int value;
        int height;
        node* parent;
        node* left;
        node* right;
    };

    // object semantics, rejects copy

    avl() 
        :root(nullptr){}

    avl(node* root) 
        :root(root){}

    avl(const avl&) = delete;
    avl& operator=(avl&) = delete;
    avl(avl&&);
    avl& operator=(avl&&);
    
    ~avl();

    node* root;

};

/**
 * \brief generates a avl from literal:
 * 
 * literal is same as the bst version
 * 
 * \param literal the literal string
 * \throw std::invalid_argument if the literal is invalid
 * \return avl the tree
 */
avl avl_from_literal(const std::string& literal);

/**
 * \brief construct the literal from the tree
 * 
 * \param tree the tree
 * \return std::string the constructed literal
 */
std::string to_literal(const avl& tree);

/**
 * \brief construct the graphviz code that represents the tree
 * 
 * \param tree the tree
 * \return std::string the graphviz code
 */
std::string to_graphviz(const avl& tree);

/**
 * \brief check the validity of the avl
 * 
 * \param tree the tree to check
 * \return std::vector<std::string> the detected violations, empty if none 
 */
std::vector<std::string> find_violation(const avl& tree);

/**
 * \brief insert an element into the tree
 * 
 * \param tree the tree
 * \param value the value to insert
 */
void insert(avl& tree, int value);

/**
 * \brief is the value in the tree?
 * 
 * \param tree the tree
 * \param value the value to find
 * \return true if exists in tree
 * \return false if not exists in tree
 */
bool search(avl& tree, int value);

/**
 * \brief remove a node from the tree
 * 
 * \param tree the tree
 * \param value the value to remove
 */
void remove(avl& tree, int value);

/**
 * \brief count the amount of element in tree
 * 
 * \param tree the tree
 * \return unsigned int element amount
 */
unsigned int count(avl& tree);

/**
 * \brief list all elements in sorted order
 * 
 * \param tree the tree
 * \return std::vector<int> the elements 
 */
std::vector<int> list_all(avl& tree);

#endif