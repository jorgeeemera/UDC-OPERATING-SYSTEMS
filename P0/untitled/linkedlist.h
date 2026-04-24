#ifndef P0_C_LINKEDLIST_H
#define P0_C_LINKEDLIST_H

#include <stdlib.h>
#include <stdbool.h>

struct node{
    void *data;
    struct node *next;
};

typedef struct node *list;
typedef struct node *pos;

void append(list l, void *cmd);

pos first(list l);

pos next(list l, pos p);

bool is_end_of_list(list l, pos p);

void *get(list l, pos p);

#endif //P0_C_LINKEDLIST_H
