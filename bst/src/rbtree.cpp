#include <algorithm>

#include "tgi/bst/rbtree.h"
#include "tgi/bst/internal/internal_rbtree.h"


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


rbtree::color color_of(const rbtree::node* n) {
    return n == nullptr ? rbtree::color::black : n->color;
}

char to_char(rbtree::color color) {
    return color == rbtree::color::black ? 'B' : 'R';
}


void find_red_root_violation(const rbtree& tree, std::vector<std::string>& violations) {
    if(color_of(tree.root)!=rbtree::color::black) {
        violations.push_back("root must not be red");
    }
}

void find_red_child_of_red_violation_internal(rbtree::node* node, std::vector<std::string>& violations) {
    if(node != nullptr) {
        if(color_of(node) == rbtree::color::red) {
            if(color_of(node->left) == rbtree::color::red) {
                violations.push_back("red node {"+std::to_string(node->value)+"}'s child {"+std::to_string(node->left->value)+"} cannot be a red node!");
            }
            if(color_of(node->right) == rbtree::color::red) {
                violations.push_back("red node {"+std::to_string(node->value)+"}'s child {"+std::to_string(node->right->value)+"} cannot be a red node!");
            }
        }
        find_red_child_of_red_violation_internal(node->left, violations);
        find_red_child_of_red_violation_internal(node->right, violations);
    }
}

void find_red_child_of_red_violation(const rbtree& tree, std::vector<std::string>& violations) {
    find_red_child_of_red_violation_internal(tree.root, violations);
}

void find_path_black_count_violation_on_node(const rbtree::node* node, std::vector<std::string>& violations) {
    int expected_black_count = -1;
    enumerate_node_to_leaf_path(node, [&](const std::vector<const rbtree::node*>& path){
        int this_black_count = std::count_if(++path.begin(), path.end(), [](auto node){return color_of(node)==rbtree::color::black;});
        if(expected_black_count == -1) {
            expected_black_count = this_black_count;
        } else {
            if(expected_black_count!=this_black_count) {
                std::string path_str;
                for (size_t i = 0; i < path.size(); i++) {
                    if(i!=0) {
                        path_str += "->";
                    }
                    path_str+= path[i] == nullptr ? std::string{"nullptr"} : "{"+std::to_string(path[i]->value)+to_char(path[i]->color)+"}";
                }
                violations.push_back("node{"+std::to_string(node->value)+"}'s path "+path_str+" has a black node count of "+std::to_string(this_black_count)+", should be "+std::to_string(expected_black_count));
            }
        }
    });
}

void find_path_black_count_violation(const rbtree& tree, std::vector<std::string>& violations) {
    enumerate_node_preorder(tree.root, [&](const rbtree::node* node){find_path_black_count_violation_on_node(node, violations);});
}

std::vector<std::string> find_violation(const rbtree& tree) {
    auto violations = std::vector<std::string>{};
    find_pointer_violation(tree, violations);
    find_value_violation(tree, violations);
    if(violations.size() == 0) {
        find_red_root_violation(tree, violations);
        find_red_child_of_red_violation(tree, violations);
        find_path_black_count_violation(tree, violations);
    }
    return violations;
}



