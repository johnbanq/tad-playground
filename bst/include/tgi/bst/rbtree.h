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

#endif