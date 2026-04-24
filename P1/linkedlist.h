#ifndef P1_C_LINKEDLIST_H
#define P1_C_LINKEDLIST_H

#include "includes.h"

struct node{
    void *data;
    struct node *next;
};

typedef struct node *list;
typedef struct node *pos;

void append(list *l, void *data);

pos first(list l);

pos next(list l, pos p);

bool is_end_of_list(list l, pos p);

void *get(list l, pos p);

void delete_first_node(list *l);

void delete_next(list l, pos p);

void delete_node(list *l, pos p);

#endif //P1_C_LINKEDLIST_H
