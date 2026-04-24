// las funciones entre puntos hay que implementarlas
#include <stdio.h>

struct node{
    char * cmd;
    struct node * next;


typedef struct node * list_cmd;
typedef struct node * pos_cmd;

void append (list_cmd l, void * cmd){
    pos p = l;
    while (p -> next != NULL) p = p -> next;
    p -> next = (pos) malloc(sizeof(struct node));
    p -> next -> cmd = cmd;
    p -> next -> next = NULL;
}

pos first(list_cmd l){
    return l;
}

pos next(list_cmd l, pos_cmd p){
    return p -> next;
}

bool is_end_of_list(list_cmd l, pos_cmd p){
    if (p == NULL) return true;
    else return false;
}   

void * get(list_cmd l, pos_cmd p){
    return p -> cmd;
}

for(pos p = first(l); p = next(l,p); !is_end_of_list(l,p)){
    puts(cmd);
}

struct node_f {
    int eol;
    char *name;
    struct node_f *next;
}

typedef struct node_f *list_f;
typedef struct node_f *pos_f;
pos_f first(list_f l){
    return l;
}
pos_f next(list_f l, pos_f p){
    return p -> next;
}
bool is_end_of_list(list_f l, pos_f p){
    if p == NULL return true;
    else return false;
}

struct file_info {
    int df;
    char *name;
}

struct file_info fi;
fi.df = df;
fi.name = name;
append (l, &fi);

void Cmd_open (char * tr[]){
    int i,df, mode = 0;

    if (tr[0] == NULL) { /*no hay parametro*/
        ListarFicherosAbiertos
        return;
    }
    
    for (i = 1; tr[i] != NULL; i++)
      if (!strcmp(tr[i],"cr")) mode |= O_CREAT;
      else if (!strcmp(tr[i],"ex")) mode |= O_EXCL;
      else if (!strcmp(tr[i],"ro")) mode |= O_RDONLY; 
      else if (!strcmp(tr[i],"wo")) mode |= O_WRONLY;
      else if (!strcmp(tr[i],"rw")) mode |= O_RDWR;
      else if (!strcmp(tr[i],"ap")) mode |= O_APPEND;
      else if (!strcmp(tr[i],"tr")) mode |= O_TRUNC; 
      else break;
      
    if ((df = open(tr[0], mode, 0777)) == - 1)
        perror ("Imposible abrir fichero");
    else{
        ...........AnadirAFicherosAbiertos (descriptor...modo...nombre....)....
        printf ("Anadida entrada a la tabla ficheros abiertos..................",......);
}

void Cmd_close (char *tr[]){ 
    int df;
    
    if (tr[0] == NULL || (df = atoi(tr[0])) < 0) { /*no hay parametro*/
      ..............ListarFicherosAbiertos............... /*o el descriptor es menor que 0*/
        return;
    }

    
    if (close(df) == -1)
        perror("Inposible cerrar descriptor");
    else
       ........EliminarDeFicherosAbiertos......
}

void Cmd_dup (char * tr[]){ 
    int df, duplicado;
    char aux[MAXNAME],*p;
    
    if (tr[0]==NULL || (df=atoi(tr[0]))<0) { /*no hay parametro*/
        ListOpenFiles(-1);                 /*o el descriptor es menor que 0*/
        return;
    }
    
    duplicado=dup(df);
    p=.....NombreFicheroDescriptor(df).......;
    sprintf (aux,"dup %d (%s)",df, p);
    .......AnadirAFicherosAbiertos......duplicado......aux.....fcntl(duplicado,F_GETFL).....;
}
