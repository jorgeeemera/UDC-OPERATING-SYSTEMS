#ifndef P0_C_CMD_H
#define P0_C_CMD_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "auxiliar.h"

#define MAXNAME 256

struct file_info {
    int fd;
    char *name;
    int mode;
};

typedef struct node *file_list;

bool procesarEntrada(char *buffer, char *trozos[]);

void Cmd_open (char * tr[]);

void Cmd_close (char *tr[]);

void Cmd_dup (char * tr[]);

#endif //P0_C_CMD_H
