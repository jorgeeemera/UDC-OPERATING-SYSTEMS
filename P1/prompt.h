#ifndef PROMPT_H
#define PROMPT_H

#include "includes.h"

#define BUFFERSIZE 1024

void imprimirPrompt();

void leerEntrada(char *buffer);

int trocearCadena(char * cadena, char * trozos[]);

#endif //PROMPT_H
