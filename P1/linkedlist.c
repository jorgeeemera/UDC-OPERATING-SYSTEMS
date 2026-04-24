#include "linkedlist.h"

void append(list *l, void *data)
{
    pos new_node = (pos) malloc(sizeof(struct node)); // Reservamos memoria para un nodo
    new_node->data = data; // El puntero de datos del nuevo nodo apunta a la misma dirección que el puntero que recibimos como parámetro
    new_node->next = NULL; // El siguiente del nodo final de la lista apunta apunta a NULL

    // Si la lista está vacía l es NULL
    if (*l == NULL)
    {
        *l = new_node;
        return;
    }

    // Empezamos la posición al principio de la lista
    pos p = *l;
    // Movemos el puntero p a next hasta que next sea NULL
    while (p->next != NULL) p = p->next;
    p->next = new_node; // p está al final de la lista, entonces hacemos que next de p apunte al nuevo nodo
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

//Esta función borra el primer nodo de la lista enlazada
//en el retorno de la función, l apunta al nuevo primer nodo
void delete_first_node(list *l)
{
    if (*l == NULL) return;
    pos p = (*l) -> next; // Hacemos que p apunte al segundo nodo de la lista (l es el primero, así que l -> next es el segundo
    free(*l); // Elimina el primer nodo
    *l = p; // Hacemos que el principio de la lista apunte al segundo nodo
}

//Recibe una posición y borra el siguiente nodo de la lista
void delete_next(list l, pos p)
{
    if(p == NULL) return; // Final de la lista, no hay siguiente
    if (p -> next == NULL) return; // Final de la lista, no hay siguiente
    pos next = p -> next; // Hacemos que next apunte al nodo que está después de p
    p -> next = next -> next; // Hacemos que el nodo siguiente de p ahora apunte al nodo siguiente de next
    free(next);
}

//Borra el nodo en la posición p
void delete_node(list *l, pos p)
{
    if(*l == NULL) return; // Lista vacía
    if(p == NULL) //Borrar el primero
    {
        delete_first_node(l);
        return;
    }
    delete_next(*l, p); //Borrar el nodo siguiente a p
}

