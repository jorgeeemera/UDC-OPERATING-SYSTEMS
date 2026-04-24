#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "auxiliar.h"
#include "cmd.h"

int main(){
    char *buffer = malloc(sizeof(char)*BUFFERSIZE);
    char *trozos[100];
    bool terminado = false; //variable que se hace true si procesarlaentrada finaliza la shell
    while (!terminado){
        imprimirPrompt();
        leerEntrada(buffer);
        terminado = procesarEntrada(buffer, trozos);
    }
    free(buffer);
}

