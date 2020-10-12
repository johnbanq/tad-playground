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


int get_height(const avl::node* n) {
    return n == nullptr ? -1 : n->height;
}

int balance_factor_of(const avl::node* n) {
    int left_height = get_height(n->left);
    int right_height = get_height(n->right);
    return left_height - right_height;
}

int compute_height(const avl::node* n) {
    return std::max(get_height(n->left), get_height(n->right)) + 1;
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
        node->height = compute_height(node);
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


/**
 * this function shouts the mismatch of recorded and actual height in tree, and will return real height of the node as reference
 */
int check_height_matches(const node* node, std::vector<std::string>& violations) {
    if(node != nullptr) {
        int left = check_height_matches(node->left, violations);
        int right = check_height_matches(node->right, violations);
        int expected_height = std::max(left, right)+1;
        if(node->height!=expected_height) {
            violations.push_back("{"+std::to_string(node->value)+"} has wrong height="+std::to_string(node->height)+", should be height="+std::to_string(expected_height));
        }
        return expected_height;
    } else {
        //so leaf will have 0
        return -1;
    }
}

/**
 * this function shouts node with bad balance factor (not +1,0,-1)
 */
void check_balance(const node* node, std::vector<std::string>& violations) {
    if(node != nullptr) {
        int factor = balance_factor_of(node);
        if(std::abs(factor)>1) {
            violations.push_back("{"+std::to_string(node->value)+"} has a bad balance factor of "+std::to_string(factor));
        }
    }
}

std::vector<std::string> find_violation(const avl& tree) {
    auto violations = std::vector<std::string>{};
    find_pointer_violation(tree, violations);
    find_value_violation(tree, violations);
    if(violations.size() == 0) {
        check_height_matches(tree.root, violations);
        check_balance(tree.root, violations);
    }
    return violations;
}


bool search(avl& tree, int value) {
    auto [parent, ref] = locate_parent_and_expected_ref(tree, value);
    return (*ref) != nullptr;
}


void left_rotate(avl::node*& n) {
    auto node = n;
    auto left = node->left;
    auto right = node->right;
    auto rightleft = right->left;
    auto rightright = right->right;

    n = right;
    right->parent = node->parent;

    node->right = rightleft;
    if(rightleft != nullptr) {
        rightleft->parent = node;
    }

    right->left = node;
    node->parent = right;

    node->height = compute_height(node);
    right->height = compute_height(right);
}

void right_rotate(avl::node*& n) {
    auto node = n;
    auto left = node->left;
    auto right = node->right;
    auto leftleft = left->left;
    auto leftright = left->right;

    n = left;
    left->parent = node->parent;
    
    node->left = leftright;
    if(leftright != nullptr) {
        leftright->parent = node;
    }

    left->right = node;
    node->parent = left;

    node->height = compute_height(node);
    left->height = compute_height(left);
}

void rebalance(avl::node*& n) {
    int factor = balance_factor_of(n);
    if(std::abs(factor) <= 1) {
        // balanced, no-op
    } else if(factor == 2) {
        int left_factor = balance_factor_of(n->left);
        if(left_factor == 1) { // left-left case
            right_rotate(n);
        } else if(left_factor == -1) { // left-right case
            left_rotate(n->left);
            right_rotate(n);
        } else {
            //impossible
            throw std::logic_error("unexpected left balance factor: "+std::to_string(factor)); 
        }
    } else if(factor == -2) {
        int right_factor = balance_factor_of(n->right);
        if(right_factor == -1) { // right-right case
            left_rotate(n);
        } else if(right_factor == 1) { // right-left case
            right_rotate(n->right);
            left_rotate(n);
        } else {
            //impossible
            throw std::logic_error("unexpected right balance factor: "+std::to_string(factor)); 
        }
    } else {
        //impossible
        throw std::logic_error("unexpected balance factor: "+std::to_string(factor));
    }
}

avl::node** ref_of(avl& tree, avl::node* node) {
    if(node->parent == nullptr) {
        if(tree.root == node) {
            return &tree.root;
        } else {
            //impossible
            throw std::logic_error("invalid node: {"+std::to_string(node->value)+"} is not root and has no parent!");
        }
    } else {
        if(node->parent->left == node) {
            return &(node->parent->left);
        } else if(node->parent->right == node) {
            return &(node->parent->right);
        } else {
            //impossible
            throw std::logic_error("invalid node: {"+std::to_string(node->value)+"} has a parent {"+std::to_string(node->parent->value)+"} but is not any of its child!");
        }
    }
}

void insert(avl& tree, int value) {
    auto [parent, ref] = locate_parent_and_expected_ref(tree, value);
    if(*ref == nullptr) {
        *ref = new node {value, 0, parent, nullptr, nullptr};
    }

    auto balancing = parent;
    while(balancing != nullptr) {
        balancing->height = compute_height(balancing);
        auto next = balancing->parent; 
        auto ref = ref_of(tree, balancing);
        rebalance(*ref);
        balancing = next;
    }
}

void perform_deletion(avl& tree, avl::node* parent, avl::node*& ref) {
    avl::node* node = ref;
    auto two_child_case = node->left != nullptr && node->right != nullptr;
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
        avl::node* victim = node->right;
        while(victim->left!=nullptr) {
            victim = victim->left;
        }
        int victim_value = victim->value;
        auto [victim_parent, victim_ref] = locate_parent_and_expected_ref(node, &(node->right), victim_value);
        perform_deletion(tree, victim_parent, *victim_ref);
        node->value = victim_value;
    }
    //two child's case has been done in deleting victim
    if(!two_child_case) {
        auto balancing = parent;
        while(balancing != nullptr) {
            balancing->height = compute_height(balancing);
            auto next = balancing->parent; 
            auto ref = ref_of(tree, balancing);
            rebalance(*ref);
            balancing = next;
        }
    }
}

void remove(avl& tree, int value) {
    auto [parent, ref] = locate_parent_and_expected_ref(tree, value);
    if(*ref != nullptr) {
        perform_deletion(tree, parent, *ref);
    }
}