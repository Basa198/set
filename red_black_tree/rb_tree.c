#include <stdlib.h>
#include <stdio.h>

#include "rb_tree.h"

void _free_node(RBnode* node) {
    if (!node) return;

    _free_node(node->_left);
    node->_left = NULL;
    _free_node(node->_right);
    node->_right = NULL;
    node->_parent = NULL;

    free(node);
}

RBnode* _find_insertion_point(RBnode* root, int value) {
    if (root->_key == value) return NULL; // don't allow duplicate values
    if (value < root->_key && root->_left) return _find_insertion_point(root->_left, value);
    else if (value > root->_key && root->_right) return _find_insertion_point(root->_right, value);
    return root;
}

RBnode* create_rb_node(int key, rb_color color) {
    RBnode* node = (RBnode*)malloc(sizeof(RBnode));
    if (!node) {
        fprintf(stderr, "Failed Memory Allocation (RBnode)\n");
        return NULL;
    }
    
    node->_color = color;
    node->_key = key;
    node->_parent = NULL;
    node->_left = NULL;
    node->_right = NULL;

    return node;
}

RBnode* rotate_dir_root(RBtree* tree, RBnode* parent_node, int dir) {
    // rotate parent_node and parent_node->_child[1 - dir] so that
    // the child becomes the parent of the parent_node
    RBnode* grandparent_node = parent_node->_parent;
    RBnode* child_node = parent_node->_child[1 - dir];
    RBnode* adopted_child = child_node->_child[dir];
    parent_node->_child[1 - dir] = adopted_child;
    if (adopted_child) adopted_child->_parent = parent_node;
    child_node->_child[dir] = parent_node;
    parent_node->_parent = child_node;
    child_node->_parent = grandparent_node;
    if (grandparent_node) {
        grandparent_node->_child[grandparent_node->_left == parent_node ? LEFT : RIGHT] = child_node;
    } else {
        tree->_root = child_node; // if grandparent is NULL, then child is the new root
    }
    return child_node;
}

RBtree* create_rb_tree() {
    RBtree* tree = (RBtree*)malloc(sizeof(RBtree));
    if (!tree) {
        fprintf(stderr, "Memory Allocation Failed (RBtree)\n");
        return NULL;
    }

    tree->_root = NULL;

    return tree;
}

RBnode* rb_search(RBtree* tree, int target) {
    if (!tree) return NULL;

    RBnode* node = tree->_root;

    while (node && node->_key != target) {
        if (node->_key > target) node = node->_left;
        else node = node->_right;
    }

    return node;
}

void free_rb_tree(RBtree* tree) {
    if (!tree) return;

    RBnode* node = tree->_root;
    _free_node(node);
    tree->_root = NULL;

    free(tree);
}

bool rb_insert(RBtree* tree, int value) {
    if (!tree) {
        fprintf(stderr, "Invalid insert: tree is NULL\n");
        return false;
    }
    RBnode* new_node = create_rb_node(value, RED);
    if (!new_node) exit(1);
    if (!tree->_root) { // if tree is empty
        tree->_root = new_node;
        return true;
    }
    RBnode* parent_node = _find_insertion_point(tree->_root, value);
    if (!parent_node) { // Will only occur when value is a duplicate
        free(new_node);
        return false;
    }; 
    int dir = parent_node->_key > new_node->_key ? LEFT : RIGHT;
    new_node->_parent = parent_node;
    parent_node->_child[dir] = new_node; 
    while (parent_node) {
        if (parent_node->_color == BLACK) return true; // no red property violation
        RBnode* grandparent_node = parent_node->_parent;
        if (!grandparent_node) { // parent is root and red
            parent_node->_color = BLACK;
            return true;
        }
        dir = grandparent_node->_left == parent_node ? LEFT : RIGHT;
        RBnode* uncle = grandparent_node->_child[1 - dir];
        if (!uncle || uncle->_color == BLACK) { // parent is red and uncle is black
            if (new_node == parent_node->_child[1 - dir]) { // if new node is an inner grandchild
                rotate_dir_root(tree, parent_node, dir);
                new_node = parent_node;
                parent_node = grandparent_node->_child[dir];
            }
            rotate_dir_root(tree, grandparent_node, 1 - dir);
            parent_node->_color = BLACK;
            grandparent_node->_color = RED;
            return true;
        }
        // both parent and uncle are red
        // make parent and uncle black and grandparent red, then grandparent becomes the new_node
        parent_node->_color = BLACK;
        uncle->_color = BLACK;
        grandparent_node->_color = RED;
        new_node = grandparent_node; 
        parent_node = grandparent_node->_parent;
    } 
    return true;
}

void case_d6(RBtree* tree, RBnode* parent, RBnode* sibling, RBnode* distant_nephew, int dir) {
    rotate_dir_root(tree, parent, dir);
    sibling->_color = parent->_color;
    parent->_color = BLACK;
    distant_nephew->_color = BLACK; 
}

bool rb_delete_node(RBtree* tree, int target) {
    if (!tree) return false;

    RBnode* node_to_remove = rb_search(tree, target);
    if (!node_to_remove) return false;

    if (node_to_remove->_left && node_to_remove->_right) { // if two non-NULL children
        // find the inorder successor of the node, switch keys and delete inorder successor
        // inorder successor will be the leftmost child of the right subtree
        RBnode* inorder_successor = node_to_remove->_right;
        while (inorder_successor->_left) {
            inorder_successor = inorder_successor->_left;
        }
        int key = inorder_successor->_key;
        rb_delete_node(tree, key);
        node_to_remove->_key = key;
        return true;
    }
    if (node_to_remove->_left || node_to_remove->_right) { // if it has only one child
        // replace the parent with the child
        int dir = node_to_remove->_left ? LEFT : RIGHT;
        node_to_remove->_child[dir]->_parent = node_to_remove->_parent;
        node_to_remove->_child[dir]->_color = BLACK;
        if (node_to_remove->_parent) { 
            int parent_dir = node_to_remove->_parent->_left == node_to_remove ? LEFT : RIGHT;
            node_to_remove->_parent->_child[parent_dir] = node_to_remove->_child[dir];
        } else {
            tree->_root = node_to_remove->_child[dir];
        }
        free(node_to_remove);
        return true;
    }
    // from here on, the node has no children
    if (tree->_root == node_to_remove) {
        tree->_root = NULL;
        free(node_to_remove);
        return true;
    }
    if (node_to_remove->_color == RED) {
        // red leaf
        // the node is NOT root (i.e has a parent)
        int dir = node_to_remove->_parent->_left == node_to_remove ? LEFT : RIGHT;
        node_to_remove->_parent->_child[dir] = NULL;
        free(node_to_remove);
        return true;
    }
    // node has no children and is black
    // if we simply delete it, it will violate the black depth property
    RBnode* parent = node_to_remove->_parent;
    int dir = parent->_left == node_to_remove ? LEFT : RIGHT;
    RBnode* sibling;
    RBnode* close_nephew;
    RBnode* distant_nephew;
    parent->_child[dir] = NULL;
    free(node_to_remove);
    
    while (parent) {
        sibling = parent->_child[1 - dir];
        close_nephew = sibling->_child[dir];
        distant_nephew = sibling->_child[1 - dir];
        if (sibling->_color == RED) { 
            rotate_dir_root(tree, parent, dir);
            parent->_color = RED;
            sibling->_color = BLACK;
            sibling = close_nephew;
            distant_nephew = sibling->_child[1 - dir];
            close_nephew = sibling->_child[dir];
        } // sibling is black
        if (distant_nephew && distant_nephew->_color == RED) {
            case_d6(tree, parent, sibling, distant_nephew, dir);
            return true;
        } // sibling is black, distant nephew is black
        if (close_nephew && close_nephew->_color == RED) {
            rotate_dir_root(tree, sibling, 1 - dir);
            sibling->_color = RED;
            close_nephew->_color = BLACK;
            distant_nephew = sibling;
            sibling = close_nephew;

            case_d6(tree, parent, sibling, distant_nephew, dir);
            return true;
        } // sibling, close_nephew, distant_nephew are black
        if (parent->_color == RED) {
            sibling->_color = RED;
            parent->_color = BLACK;
            return true;
        } // parent, sibling, close_nephew, distant_nephew are black

        sibling->_color = RED;
        node_to_remove = parent;
        parent = node_to_remove->_parent;        
        if (parent) {
            dir = parent->_left == node_to_remove ? LEFT : RIGHT;
        }
    }
    return true;
}