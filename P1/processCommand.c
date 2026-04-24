#include "processCommand.h"


bool procesarEntrada(char *buffer, char *tr[]){
    size_t longitud_command = strlen(buffer); // Longitud de la entrada
    char *cmd = malloc((longitud_command + 1)*sizeof(char)); //Reservo memoria para un string local en el que copiar el buffer
    strcpy(cmd, buffer); //Copiar buffer a variable local
    trocearCadena(cmd, tr); //Rellena el array tr con un puntero al inicio de cada palabra terminada por \0

    // Comparar el primer trozo con cada comando disponible y ejecutar la operación correspondiente
    // Si el argumento tiene parámetros pasamos un puntero a la segunda posición del array de trozos
    if(strcmp(tr[0], "open") == 0){
        cmdOpen(&tr[1]);
    }

    else if (strcmp(tr[0], "close") == 0){
        cmdClose(&tr[1]);
    }

    else if (strcmp(tr[0], "dup") == 0){
        cmdDup(&tr[1]);
    }

    else if (strcmp(tr[0], "historic") == 0)
    {
        cmdHistoric(&tr[1]);
    }

    else if (strcmp(tr[0], "authors") == 0){
        cmdAuthors(&tr[1]);
    }

    else if (strcmp(tr[0], "pid") == 0) {
        getPid(tr);
    }

    else if (strcmp(tr[0], "ppid") == 0){
        getPPid(tr);
    }

    else if (strcmp(tr[0], "date") == 0){
        getDate(tr);
    }

    else if (strcmp(tr[0], "infosys") == 0){
        infoSys(tr);
    }

    else if(strcmp(tr[0], "cd") == 0){
        cmdCd(&tr[1]);
    }

    else if (strcmp(tr[0], "help") == 0){
        help(tr);
    }

    else if(strcmp(tr[0], "quit") == 0 || strcmp(tr[0], "exit") == 0 || strcmp(tr[0], "bye") == 0)
    {
        free(cmd);
        return true;
    }

    else if(strcmp(tr[0], "makefile") == 0){
        makeFile(tr);
    }

    else if(strcmp(tr[0], "makedir") == 0){
        makeDir(tr);
    }

    else if(strcmp(tr[0], "listfile") == 0){
        listFile(tr);
    }

    else if(strcmp(tr[0], "cwd") == 0){
        cwd(tr);
    }

    else if(strcmp(tr[0], "listdir") == 0){
        listdir(tr);
    }

    else if(strcmp(tr[0], "reclist") == 0){
        reclist(tr);
    }

    else if(strcmp(tr[0], "revlist") == 0){
        revlist(tr);
    }

    else if(strcmp(tr[0], "erase") == 0){
        erase(tr);
    }

    else if(strcmp(tr[0], "delrec") == 0){
        delrec(tr);
    }

    free(cmd);
    return false;
}