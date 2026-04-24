#include "linkedlist.h"

void append(list l, void *data){
    pos p = l;
    while(p -> next != NULL) p = p -> next;
    p -> next = (pos) malloc(sizeof(struct node));
    p -> next -> data = data;
    p -> next -> next = NULL;
}

pos first(list l){
    return l;
}

pos next(list l, pos p){
    return p -> next;
}

bool is_end_of_list(list l, pos p){
    if (p == NULL) return true;
    return false;
}

void *get(list l, pos p){
    return p -> data;
}