#ifndef P0_C_AUXILIAR_H
#define P0_C_AUXILIAR_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "auxiliar.h"

#define BUFFERSIZE 1024

void imprimirPrompt();

void leerEntrada(char *buffer);



int TrocearCadena(char * cadena, char * trozos[]);

#endif //P0_C_AUXILIAR_H
