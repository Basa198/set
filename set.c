#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "set.h"

Set* create_set() {
    RBtree* tree = create_rb_tree();
    if (!tree) return NULL;
    Set* set = (Set*)malloc(sizeof(Set));
    if (!set) {
        fprintf(stderr, "Memory Allocation Failed (set)\n");
        return NULL;
    }
    set->size = 0;
    set->_tree = tree;

    return set;
}

void free_set(Set* set) {
    if (!set) return;
    free_rb_tree(set->_tree);
    free(set);
}

void set_insert(Set* set, int val) {
    if (!set) return;
    if (rb_insert(set->_tree, val)) {
        set->size += 1;
    }
} 

void set_remove(Set* set, int target) {
    if (!set) return;
    if (rb_delete_node(set->_tree, target)) {
        set->size -= 1;
    }
}

bool set_search(Set* set, int target) {
    if (!set) return false;
    return rb_search(set->_tree, target) ? true : false;
}