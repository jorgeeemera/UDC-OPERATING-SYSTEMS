#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "includes.h"

struct node{
    void *data; // Puntero genérico que puede apuntar a cualquier tipo de datos
    struct node *next; // Puntero que apunta al siguiente nodo de la lista.
};

typedef struct node *list; // Define list como un puntero al primer nodo de la lista enlazada
typedef struct node *pos; // Define pos como un puntero a un nodo en la lista, para recorrer o manipular la lista.

void append(list *l, void *data);

pos first(list l);

pos next(list l, pos p);

bool is_end_of_list(list l, pos p);

void *get(list l, pos p);

void delete_first_node(list *l);

void delete_next(list l, pos p);

void delete_node(list *l, pos p);

#endif //LINKEDLIST_H
