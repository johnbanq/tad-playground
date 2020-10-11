#include "tgi/bst/naive_bst.h"
#include "tgi/bst/hex_util.h"
#include "tgi/bst/internal_naive_bst.h"

#include <limits>

using node = bst::node;

void delete_tree(node* n) {
    if(n!=nullptr) {
        delete_tree(n->left);
        delete_tree(n->right);
        delete n;
    }
}

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


struct bst_literal_parser: public literal_parser<bst::node> {

    bst_literal_parser(const std::string& literal, size_t offset)
        : literal_parser<bst::node>(literal, offset) {}

    bst::node* build_node(int value, bst::node* parent) override {
        return new bst::node{value, parent, nullptr, nullptr};
    }

    bst::node* set_child(bst::node* node, bst::node* left, bst::node* right) override {
        node->left = left;
        node->right = right;
        return node;
    }

};

bst bst_from_literal(const std::string& literal) {
    bst_literal_parser parser{literal, 0};
    return bst{parser.parse(nullptr)};
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


void to_graphviz(const bst::node* root, std::string& buffer);

std::string to_graphviz(const bst& tree) {
    std::string buffer;
    buffer += "digraph bst{";
    to_graphviz(tree.root, buffer);
    buffer += "}";
    return buffer;
}

void to_graphviz(const bst::node* root, std::string& buffer) {
    if(root!=nullptr) {
        buffer += node_stmt(root->value, root);
        if(root->parent != nullptr) {
            buffer += edge_stmt(root->value, root->parent->value, "parent");
        }
        if(root->left != nullptr) {
            buffer += edge_stmt(root->value, root->left->value, "left");
        }
        if(root->right != nullptr) {
            buffer += edge_stmt(root->value, root->right->value, "right");
        }
        to_graphviz(root->left, buffer);
        to_graphviz(root->right, buffer);
    }
}


/**
 * find violations in pointer pointing, eg: incorrect parent
 */
void find_pointer_violation(const bst::node* node, std::vector<std::string>& violations);

/**
 * find violation in value placement, eg bigger value in left branch
 * the node is expected to be in range (min, max]
 */
void find_value_violation(const bst::node* node, int min, int max, std::vector<std::string>& violations);

std::vector<std::string> find_violation(const bst& tree) {
    auto violations = std::vector<std::string>{};

    if(tree.root != nullptr && tree.root->parent != nullptr) {
        violations.push_back("root's parent must be null!");
    }
    find_pointer_violation(tree.root, violations);
    
    find_value_violation(tree.root, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), violations);
    return violations;
}

void find_pointer_violation(const bst::node* node, std::vector<std::string>& violations) {
    if(node != nullptr) {
        if(node->left!=nullptr && node->left->parent != node) {
            violations.push_back(parent_violation(node, node->left));
        }
        find_pointer_violation(node->left, violations);
        if(node->right!=nullptr && node->right->parent != node) {
            violations.push_back(parent_violation(node, node->right));
        }
        find_pointer_violation(node->right, violations);
    }
}

void find_value_violation(const bst::node* node, int min, int max, std::vector<std::string>& violations) {
    if(node!=nullptr) {
        if(!(min < node->value && node->value <= max)) {
            auto min_repr = (min == std::numeric_limits<int>::min() ? std::string{"int_min"} : std::to_string(min));
            auto max_repr = (max == std::numeric_limits<int>::max() ? std::string{"int_max"} : std::to_string(max));
            violations.push_back("{"+std::to_string(node->value)+"} is in wrong place: it should must be in range ("+min_repr+","+max_repr+"]");
        } else {
            find_value_violation(node->left, min, node->value, violations);
            find_value_violation(node->right, node->value, max, violations);
        }
    }
}


std::pair<node*, node**> locate_parent_and_expected_ref(bst::node* parent, bst::node** source_ref, int value) {
    bst::node* node = *source_ref;
    if(node == nullptr) {
        return std::make_pair(parent, source_ref);
    } else {
        if(value < node->value) {
            return locate_parent_and_expected_ref(node, &(node->left), value);
        } else if(value == node->value) {
            return std::make_pair(parent, source_ref);
        } else {
            return locate_parent_and_expected_ref(node, &(node->right), value);
        }
    }
} 

std::pair<node*, node**> locate_parent_and_expected_ref(bst& tree, int value) {
    return locate_parent_and_expected_ref(nullptr, &(tree.root), value);
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