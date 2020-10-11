#include "tgi/bst/naive_bst.h"
#include "tgi/bst/hex_util.h"
#include "tgi/bst/internal_naive_bst.h"

#include <limits>

using node = bst::node;

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
