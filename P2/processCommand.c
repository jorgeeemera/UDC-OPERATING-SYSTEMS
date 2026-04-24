#include "processCommand.h"


bool procesarEntrada(const char *buffer, char *tr[], char *envp[]){

    // Si el buffer contiene solo un salto de línea o está vacío, no procesar
    if (buffer == NULL || strcmp(buffer, "\n") == 0 || strlen(buffer) == 0) {
        return false;
    }

    const size_t longitud_command = strlen(buffer); // Longitud de la entrada
    char *cmd = malloc((longitud_command + 1)*sizeof(char)); // Reservo memoria para un string local en el que copiar el buffer
    strcpy(cmd, buffer); // Copiar buffer a variable local
    trocearCadena(cmd, tr); // Rellena el array tr con un puntero al inicio de cada palabra terminada por \0

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

    else if (strcmp(tr[0], "allocate") == 0){
        do_Allocate(&tr[1]);
    }

    else if (strcmp(tr[0], "deallocate") == 0){
        do_Deallocate(&tr[1]);
    }

    else if (strcmp(tr[0], "memfill") == 0){
        Cmd_MemFill(&tr[1]);
    }

    else if (strcmp(tr[0], "memdump") == 0){
        Cmd_MemDump(&tr[1]);
    }

    else if (strcmp(tr[0], "memory") == 0){
        Cmd_Memory(&tr[1]);
    }

    else if (strcmp(tr[0], "recurse") == 0){
        recurse(&tr[1]);
    }

    else if(strcmp(tr[0], "readfile") == 0){
        Cmd_ReadFile(&tr[1]);
    }

    else if(strcmp(tr[0], "writefile") == 0){
        Cmd_WriteFile(&tr[1]);
    }

    else if(strcmp(tr[0], "read") == 0){
        Cmd_Read(&tr[1]);
    }

    else if(strcmp(tr[0], "write") == 0){
        Cmd_Write(&tr[1]);
    }

    else if(strcmp(tr[0], "getuid") == 0){
        Cmd_GetUid(tr);
    }

    else if(strcmp(tr[0], "setuid") == 0){
        Cmd_SetUid(&tr[1]);
    }

    else if(strcmp(tr[0], "showvar") == 0){
        Cmd_ShowVar(&tr[1], envp);
    }

    else if(strcmp(tr[0], "changevar") == 0){
        Cmd_ChangeVar(&tr[1], envp);
    }

    else if(strcmp(tr[0], "subsvar") == 0){
        Cmd_SubsVar(&tr[1], envp);
    }

    else if(strcmp(tr[0], "environ") == 0){
        Cmd_Environ(&tr[1], envp);
    }

    else if(strcmp(tr[0], "fork") == 0){
        Cmd_fork(tr);
    }

    else if(strcmp(tr[0], "search") == 0){
        Cmd_Search(&tr[1]);
    }

    else if(strcmp(tr[0], "exec") == 0){
        Cmd_Exec(&tr[1]);
    }

    else if(strcmp(tr[0], "execpri") == 0){
        Cmd_ExecPri(&tr[1]);
    }

    else if(strcmp(tr[0], "fg") == 0){
        int result = Cmd_fg(&tr[1]);
        if (result == -2){
            free(cmd);
            return true;
        }
    }

    else if(strcmp(tr[0], "fgpri") == 0){
        int result = Cmd_fgpri(&tr[1]);
        if (result == -2){
            free(cmd);
            return true;
        }
    }

    else if(strcmp(tr[0], "back") == 0){
        Cmd_Back(&tr[1]);
    }

    else if(strcmp(tr[0], "backpri") == 0){
        Cmd_BackPri(&tr[1]);
    }

    else if(strcmp(tr[0], "listjobs") == 0){
        Cmd_Listjobs();
    }

    else if(strcmp(tr[0], "deljobs") == 0){
        Cmd_DelJobs(&tr[1]);
    }

    // Comando desconocido
    else {
        int result = Cmd_fg(&tr[1]);
        if (result == -2){
            free(cmd);
            return true;
        }
    }

    free(cmd);
    return false;
}