#include "tgi/bst/naive_bst.h"
#include "tgi/bst/hex_util.h"
#include "tgi/bst/internal/internal_bst_common.h"

#include <limits>


using node = bst::node;


bst::bst(bst&& other)
    :root(nullptr) {
    this->root = other.root;
    other.root = nullptr;
}

bst& bst::operator=(bst&& other) {
    delete_tree(this->root);
    this->root = other.root;
    other.root = nullptr;
    return *this;
}

bst::~bst() {
    delete_tree(root);
}


struct bst_literal_parser: public literal_parser<bst::node, int> {

    bst_literal_parser(const std::string& literal, size_t offset)
        : literal_parser<bst::node, int>(literal, offset) {}

    virtual int parse_value() override {
        return parse_int();
    }

    bst::node* build_node(int value, bst::node* left, bst::node* right) override {
        auto node = new bst::node{value, nullptr, nullptr, nullptr};
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

bst bst_from_literal(const std::string& literal) {
    bst_literal_parser parser{literal, 0};
    return bst{parser.parse()};
}

struct bst_literal_writer: public literal_writer<bst::node> {

    virtual void write_value(const bst::node* root, std::string& str) override {
        str += std::to_string(root->value);
    }

};

std::string to_literal(const bst& tree) {
    bst_literal_writer writer;
    std::string str;
    writer.write(tree.root, str);
    return str;
}

struct bst_graphviz_writer: public graphviz_writer<bst::node> {

    virtual void write_node(const bst::node* root, std::string& buffer) override {
        buffer += node_stmt(root->value, root);
    }

    virtual void write_edge(const bst::node* from, const bst::node* to, const std::string& type, std::string& buffer) override {
        buffer += edge_stmt(from->value, to->value, type);
    }

};

std::string to_graphviz(const bst& tree) {
    bst_graphviz_writer writer;
    return writer.write(tree.root);
}


std::vector<std::string> find_violation(const bst& tree) {
    auto violations = std::vector<std::string>{};
    find_pointer_violation(tree, violations);
    find_value_violation(tree, violations);
    return violations;
}


bool search(bst& tree, int value) {
    auto [parent, ref] = locate_parent_and_expected_ref(tree, value);
    return (*ref) != nullptr;
}

void insert(bst& tree, int value) {
    auto [parent, ref] = locate_parent_and_expected_ref(tree, value);
    if(*ref == nullptr) {
        *ref = new node {value, parent, nullptr, nullptr};
    }
}

void perform_deletion(bst::node* parent, bst::node*& ref) {
    bst::node* node = ref;
    if(node->left == nullptr && node->right == nullptr) { // 0 child case
        ref = nullptr;
        delete node;
    }  else if(node->left != nullptr && node->right == nullptr) { // 1 child case
        ref = node->left;
        ref->parent = parent;
        delete node;
    } else if(node->left == nullptr && node->right != nullptr) { // 1 child case
        ref = node->right;
        ref->parent = parent;
        delete node;
    } else { // 2 child case
        //we replace the node to delete with the right subtree's smallest element
        bst::node* victim = node->right;
        while(victim->left!=nullptr) {
            victim = victim->left;
        }
        int victim_value = victim->value;
        auto [victim_parent, victim_ref] = locate_parent_and_expected_ref(node, &(node->right), victim_value);
        perform_deletion(victim_parent, *victim_ref);
        node->value = victim_value;
    }
}

void remove(bst& tree, int value) {
    auto [parent, ref] = locate_parent_and_expected_ref(tree, value);
    if(*ref != nullptr) {
        perform_deletion(parent, *ref);
    }
}