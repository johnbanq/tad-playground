#ifndef TGI_BST_RBTREE_H_
#define TGI_BST_RBTREE_H_

#include<string>
#include<stdexcept>
#include<vector>

struct rbtree {
    
    enum class color {
        red, black
    };

    struct node {
        int value;
        color color;
        node* parent;
        node* left;
        node* right;
    };

    // object semantics, rejects copy

    rbtree() 
        :root(nullptr){}

    rbtree(node* root) 
        :root(root){}

    rbtree(const rbtree&) = delete;
    rbtree& operator=(rbtree&) = delete;
    rbtree(rbtree&&);
    rbtree& operator=(rbtree&&);
    
    ~rbtree();

    node* root;

};

/**
 * \brief generates a rbtree from literal:
 * 
 * literal is same as the bst version, but the node value must end with character R or B to indicate node color: 
 *  (7B:(1B):(9B)) means a (7:(1):(9)) with all nodes black  
 * 
 * \param literal the literal string
 * \throw std::invalid_argument if the literal is invalid
 * \return rbtree the tree
 */
rbtree rbtree_from_literal(const std::string& literal);

/**
 * \brief construct the literal from the tree
 * 
 * \param tree the tree
 * \return std::string the constructed literal
 */
std::string to_literal(const rbtree& tree);

#endif