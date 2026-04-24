#include "cmd.h"

bool procesarEntrada(char *buffer, char *trozos[]){
    int numpalabras = TrocearCadena(buffer, trozos);
    if(trozos[0] == NULL)
        return false;
    if(strcmp(trozos[0], "open") == 0){
        Cmd_open(&trozos[1]);
    }
    else if(strcmp(trozos[0], "quit") == 0 || strcmp(trozos[0], "exit") == 0 || strcmp(trozos[0], "bye") == 0)
        return true;

    return false;
}

void Cmd_open (char * tr[])
{
    int i,df, mode=0;

    if (tr[0]==NULL) { /*no hay parametro*/
        //..............ListarFicherosAbiertos...............
        return;
    }
    for (i=1; tr[i]!=NULL; i++)
        if (!strcmp(tr[i],"cr")) mode|= O_CREAT;
        else if (!strcmp(tr[i],"ex")) mode|= O_EXCL;
        else if (!strcmp(tr[i],"ro")) mode|= O_RDONLY;
        else if (!strcmp(tr[i],"wo")) mode|= O_WRONLY;
        else if (!strcmp(tr[i],"rw")) mode|= O_RDWR;
        else if (!strcmp(tr[i],"ap")) mode|= O_APPEND;
        else if (!strcmp(tr[i],"tr")) mode|= O_TRUNC;
        else break;

    if ((df=open(tr[0],mode,0777))==-1)
        perror ("Imposible abrir fichero");
    else {
        //AnadirAFicherosAbiertos(df, mode, tr[0]);
        printf("Anadida entrada a la tabla ficheros abiertos: %s", df);
    }
}

void Cmd_close (char *tr[]) {
    int df;

    if (tr[0] == NULL || (df = atoi(tr[0])) < 0) { /*no hay parametro*/
        //..............ListarFicherosAbiertos............... /*o el descriptor es menor que 0*/
        return;
    }


    if (close(df) == -1)
        perror("Imposible cerrar descriptor");
    else{
        //........EliminarDeFicherosAbiertos......
    }
}

void Cmd_dup (char * tr[]){
    int df, duplicado;
    char aux[MAXNAME],*p;

    if (tr[0]==NULL || (df=atoi(tr[0]))<0) { /*no hay parametro*/
        //ListOpenFiles(-1);                 /*o el descriptor es menor que 0*/
        return;
    }

    duplicado=dup(df);
    //p=.....NombreFicheroDescriptor(df).......;
    sprintf (aux,"dup %d (%s)",df, p);
    //.......AnadirAFicherosAbiertos......duplicado......aux.....fcntl(duplicado,F_GETFL).....;
}

