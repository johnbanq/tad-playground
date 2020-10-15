#include <algorithm>

#include "tgi/bst/rbtree.h"
#include "tgi/bst/internal_rbtree.h"


using node = rbtree::node;


rbtree::rbtree(rbtree&& other)
    :root(nullptr) {
    this->root = other.root;
    other.root = nullptr;
}

rbtree& rbtree::operator=(rbtree&& other) {
    delete_tree(this->root);
    this->root = other.root;
    other.root = nullptr;
    return *this;
}

rbtree::~rbtree() {
    delete_tree(root);
}


struct rbtree_literal_parser: public literal_parser<rbtree::node, std::pair<int, rbtree::color>> {

    rbtree_literal_parser(const std::string& literal, size_t offset)
        : literal_parser<rbtree::node, std::pair<int, rbtree::color>>(literal, offset) {}

    virtual std::pair<int, rbtree::color> parse_value() override {
        auto i = parse_int();
        rbtree::color color;
        if(try_consume('R')) {
            color = rbtree::color::red;
        } else if(try_consume('B')) {
            color = rbtree::color::black;
        } else {
            fail("char R or B");
        }
        return std::make_pair(i, color);
    }

    rbtree::node* build_node(std::pair<int, rbtree::color> value, rbtree::node* left, rbtree::node* right) override {
        auto node = new rbtree::node{value.first, value.second, nullptr, nullptr, nullptr};
        node->left = left;
        if(left!=nullptr) {
            left->parent = node;
        }
        node->right = right;
        if(right!=nullptr) {
            right->parent = node;
        }
        return node;
    }

};

rbtree rbtree_from_literal(const std::string& literal) {
    rbtree_literal_parser parser{literal, 0};
    return rbtree{parser.parse()};
}


struct rbtree_literal_writer: public literal_writer<rbtree::node> {

    virtual void write_value(const rbtree::node* root, std::string& str) override {
        str += std::to_string(root->value);
        str += root->color == rbtree::color::black ? 'B' : 'R';
    }

};

std::string to_literal(const rbtree& tree) {
    rbtree_literal_writer writer;
    std::string str;
    writer.write(tree.root, str);
    return str;
}

struct rbtree_graphviz_writer: public graphviz_writer<rbtree::node> {

    virtual void write_node(const rbtree::node* node, std::string& buffer) override {
        buffer += rbtree_node_stmt(node);
    }

    virtual void write_edge(const rbtree::node* from, const rbtree::node* to, const std::string& type, std::string& buffer) override {
        buffer += edge_stmt(from->value, to->value, type);
    }

};

std::string to_graphviz(const rbtree& tree) {
    rbtree_graphviz_writer writer;
    return writer.write(tree.root);
}