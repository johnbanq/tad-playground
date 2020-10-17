#include <algorithm>

#include "tgi/bst/avl.h"
#include "tgi/bst/internal/internal_avl.h"
#include "tgi/bst/internal/internal_bst_common.h"


using node = avl::node;


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

struct avl_literal_parser: public literal_parser<avl::node, int> {

    avl_literal_parser(const std::string& literal, size_t offset)
        : literal_parser<avl::node, int>(literal, offset) {}

    virtual int parse_value() override {
        return parse_int();
    }

    avl::node* build_node(int value, avl::node* left, avl::node* right) override {
        auto node = new avl::node{value, 0, nullptr, nullptr, nullptr};
        node->left = left;
        if(left!=nullptr) {
            left->parent = node;
        }
        node->right = right;
        if(right!=nullptr) {
            right->parent = node;
        }
        node->height = compute_height(node);
        return node;
    }

};

avl avl_from_literal(const std::string& literal) {
    avl_literal_parser parser{literal, 0};
    return avl{parser.parse()};
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


void avl_left_rotate(avl::node*& n) {
    auto node = n;
    auto right = node->right;
    left_rotate(n);
    node->height = compute_height(node);
    right->height = compute_height(right);
}

void avl_right_rotate(avl::node*& n) {
    auto node = n;
    auto left = node->left;
    right_rotate(n);
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
            avl_right_rotate(n);
        } else if(left_factor == -1) { // left-right case
            avl_left_rotate(n->left);
            avl_right_rotate(n);
        } else if(left_factor == 0) { // tie, whatever you wish
            avl_right_rotate(n);
        } else {
            //impossible
            throw std::logic_error("unexpected left balance factor on {"+std::to_string(n->left->value)+"}: "+std::to_string(left_factor)); 
        }
    } else if(factor == -2) {
        int right_factor = balance_factor_of(n->right);
        if(right_factor == -1) { // right-right case
            avl_left_rotate(n);
        } else if(right_factor == 1) { // right-left case
            avl_right_rotate(n->right);
            avl_left_rotate(n);
        } else if(right_factor == 0) { // tie, whatever you wish
            avl_left_rotate(n);
        } else {
            //impossible
            throw std::logic_error("unexpected right balance factor on {"+std::to_string(n->right->value)+"}: "+std::to_string(right_factor)); 
        }
    } else {
        //impossible
        throw std::logic_error("unexpected balance factor on {"+std::to_string(n->value)+"}: "+std::to_string(factor));
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


unsigned int count_internal(avl::node* root) {
    if(root != nullptr) {
        return 1 + count_internal(root->left) + count_internal(root->right); 
    } else {
        return 0;
    }
}

unsigned int count(avl& tree) {
    return count_internal(tree.root);
}


void list_all_internal(avl::node* root, std::vector<int>& elems) {
    if(root!=nullptr) {
        list_all_internal(root->left, elems);
        elems.push_back(root->value);
        list_all_internal(root->right, elems);
    }
}

std::vector<int> list_all(avl& tree) {
    std::vector<int> elems;
    list_all_internal(tree.root, elems);
    return elems;
}