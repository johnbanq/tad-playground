#ifndef TGI_BST_INTERNAL_BST_GRAPHVIZ_H_
#define TGI_BST_INTERNAL_BST_GRAPHVIZ_H_

#include<string>

#include "tgi/bst/hex_util.h"

// graphviz helper //

template<typename node_type>
struct graphviz_writer {

    virtual ~graphviz_writer() = default;

    std::string write(const node_type* root) {
        std::string buffer;
        buffer += "digraph bst{";
        write(root, buffer);
        buffer += "}";
        return buffer;
    }

    void write(const node_type* root, std::string& buffer) {
        if(root!=nullptr) {
            write_node(root, buffer);
            if(root->parent != nullptr) {
                write_edge(root, root->parent, "parent", buffer);
            }
            if(root->left != nullptr) {
                write_edge(root, root->left, "left", buffer);
            }
            if(root->right != nullptr) {
                write_edge(root, root->right, "right", buffer);
            }
            write(root->left, buffer);
            write(root->right, buffer);
        }
    }

    virtual void write_node(const node_type* root, std::string& buffer) = 0;

    virtual void write_edge(const node_type* from, const node_type* to, const std::string& type, std::string& buffer) = 0;

};

/**
 * turns pointer address to 2Byte value for ease of presentation
 */
static std::string addr_string(const void* addr) {
    return int_to_hex((uint16_t(uint64_t(addr))));
}

/**
 * generic write function for write_node
 */
static std::string node_stmt(int value, const void* addr) {
    return std::to_string(value)+"[label=\""+std::to_string(value)+"("+addr_string(addr)+")\"];";
}

/**
 * generic write function for write_edge
 */
static std::string edge_stmt(int from, int to, const std::string& edge) {
    auto stmt = std::to_string(from)+" -> "+std::to_string(to)+" [label=\""+edge+"\"]";
    if(edge == "parent") {
        stmt += "[color=grey][fontcolor=grey]";
    }
    stmt += ";";
    return stmt;
}

#endif