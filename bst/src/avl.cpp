#include <algorithm>

#include "tgi/bst/avl.h"
#include "tgi/bst/internal_avl.h"
#include "tgi/bst/internal_naive_bst.h"


using node = avl::node;

void delete_tree(node* n) {
    if(n!=nullptr) {
        delete_tree(n->left);
        delete_tree(n->right);
        delete n;
    }
}

avl::avl(avl&& other)
    :root(nullptr) {
    this->root = other.root;
    other.root = nullptr;
}

avl& avl::operator=(avl&& other) {
    delete_tree(this->root);
    this->root = other.root;
    other.root = nullptr;
    return *this;
}

avl::~avl() {
    delete_tree(root);
}


struct avl_literal_parser: public literal_parser<avl::node> {

    avl_literal_parser(const std::string& literal, size_t offset)
        : literal_parser<avl::node>(literal, offset) {}

    avl::node* build_node(int value, avl::node* parent) override {
        return new avl::node{value, 0, parent, nullptr, nullptr};
    }

    avl::node* set_child(avl::node* node, avl::node* left, avl::node* right) override {
        node->left = left;
        node->right = right;
        node->height = std::max(left==nullptr?-1:left->height, right==nullptr?-1:right->height) + 1;
        return node;
    }

};

avl avl_from_literal(const std::string& literal) {
    avl_literal_parser parser{literal, 0};
    return avl{parser.parse(nullptr)};
}

struct avl_literal_writer: public literal_writer<avl::node> {

    virtual void write_value(const avl::node* root, std::string& str) override {
        str += std::to_string(root->value);
    }

};

std::string to_literal(const avl& tree) {
    avl_literal_writer writer;
    std::string str;
    writer.write(tree.root, str);
    return str;
}

struct avl_graphviz_writer: public graphviz_writer<avl::node> {

    virtual void write_node(const avl::node* node, std::string& buffer) override {
        buffer += avl_node_stmt(node);
    }

    virtual void write_edge(const avl::node* from, const avl::node* to, const std::string& type, std::string& buffer) override {
        buffer += edge_stmt(from->value, to->value, type);
    }

};

std::string to_graphviz(const avl& tree) {
    avl_graphviz_writer writer;
    return writer.write(tree.root);
}
