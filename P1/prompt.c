#include "prompt.h"

void imprimirPrompt(){
    printf("-> ");
}
void leerEntrada(char *buffer){
    fgets(buffer, BUFFERSIZE, stdin); // Lee una entrada de hasta BUFFERSIZE caracteres, y se guarda en buffer
}

int trocearCadena(char * cadena, char * trozos[]){
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL) // Almacena la primera palabra de la entrada en trozos[0]
        return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL) // Almacena las sucesivas palabras de la entrada en trozos[1], trozos[2]...
        i++;
    return i;
}

