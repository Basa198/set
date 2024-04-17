#ifndef _CUS_SET_
#define _CUS_SET_

#include <stdbool.h>
#include "./red_black_tree/rb_tree.h"

typedef struct Set {
    int size;
    RBtree* _tree;
} Set;

Set* create_set();
void set_insert(Set* set, int val);
bool set_search(Set* set, int target);
void set_remove(Set* set, int target);
void free_set(Set* set);

#endif