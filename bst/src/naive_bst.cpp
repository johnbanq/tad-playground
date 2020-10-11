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

struct literal_parser {

    bool try_consume(char ch) {
        if(literal[offset]==ch) {
            offset += 1;
            return true;
        } else {
            return false;   
        }
    }

    void consume(char ch) {
        if(!try_consume(ch)) {
            throw std::invalid_argument{std::string{"unexpected charactor ["}+literal[offset]+"] found at literal offset="+std::to_string(offset)+", expecting char"+ch};    
        }
    }

    bool try_consume(const std::string& str) {
        if(literal.substr(offset, str.size())==str) {
            offset += str.size();
            return true;
        } else {
            return false;   
        }
    }

    void consume(const std::string& str) {
        if(!try_consume(str)) {
            throw std::invalid_argument{std::string{"unexpected charactor ["}+literal[offset]+"] found at literal offset="+std::to_string(offset)+", expecting string "+str};    
        }
    }

    int parse_int() {
        size_t parsed = 0;
        int value = std::stoi(literal.substr(offset, literal.size()), &parsed, 10);
        offset += parsed;
        return value;
    }
    
    const std::string& literal;
    size_t offset;

};


node* parse(literal_parser& parser, bst::node* parent);

bst from_literal(const std::string& literal) {
    literal_parser parser{literal, 0};
    return bst{parse(parser, nullptr)};
}

node* parse(literal_parser& parser, bst::node* parent) {
    if(parser.try_consume("null")) {
        return nullptr;
    } else {
        parser.consume('(');
        int value = parser.parse_int();
        if(parser.try_consume(')')) {
            return new node{value, parent, nullptr, nullptr};
        } else {
            auto result = new node{value, parent, nullptr, nullptr};
            parser.consume(':');
            result->left = parse(parser, result);
            parser.consume(':');
            result->right = parse(parser, result);
            parser.consume(')');
            return result;
        }
    }
}


void write(const node* root, std::string& str);

std::string to_literal(const bst& tree) {
    std::string str;
    write(tree.root, str);
    return str;
}

void write(const node* root, std::string& str) {
    if(root == nullptr) { // null root case
        str += "null";
    } else {
        str += "(";
        str += std::to_string(root->value);
        if(root->left != nullptr || root->right != nullptr) { // at least 1 child
            str += ":";
            write(root->left, str);
            str += ":";
            write(root->right, str);
        }
        str += ")";
    }
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


void insert_internal(node* n, int value) {
    if(value < n->value) {
        if(n->left == nullptr) {
            n->left = new node{value, n, nullptr, nullptr};
        } else {
            insert_internal(n->left, value);
        }
    } else if(value == n->value ) {
        return; // no need to insert
    } else {  /* if(value > node->value) */
        if(n->right == nullptr) {
            n->right = new node{value, n, nullptr, nullptr};
        } else {
            insert_internal(n->right, value);
        }
    }
}

void insert(bst& tree, int value) {
    if(tree.root == nullptr) {
        tree.root = new node {value, nullptr, nullptr, nullptr};
    } else {
        insert_internal(tree.root, value);
    }
}

bool search_internal(node* node, int value) {
    if(node == nullptr) {
        return false;
    } else {
        if(value < node->value) {
            return search_internal(node->left, value);
        } else if(value == node->value) {
            return true;
        } else {  /* if(value > node->value) */
            return search_internal(node->right, value);
        }
    }
}

bool search(bst& tree, int value) {
    return search_internal(tree.root, value);
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

void perform_deletion(bst::node* parent, bst::node*& ref);

int steal_value_from_smallest_in_right_subtree(node* node) {
    bst::node* result = node->right;
    while(result->left!=nullptr) {
        result = result->left;
    }
    int stolen = result->value;
    auto [parent, ref] = locate_parent_and_expected_ref(node, &(node->right), stolen);
    perform_deletion(parent, *ref);
    return stolen;
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
        node->value = steal_value_from_smallest_in_right_subtree(node);
    }
}

void remove(bst& tree, int value) {
    auto [parent, ref] = locate_parent_and_expected_ref(tree, value);
    if(*ref != nullptr) {
        perform_deletion(parent, *ref);
    }
}