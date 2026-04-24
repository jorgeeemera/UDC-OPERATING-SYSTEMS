#include "processCommand.h"
#include "auxfunctions.h"

void cmdOpen(char *tr[]) {
    int i, df, mode = 0;

    if (tr[0] == NULL) { // No hay parámetro, listar archivos abiertos
        listarFicherosAbiertos();
        return;
    }

    // Recorremos los parámetros después del nombre del archivo (a partir de tr[1])
    // añadiendo los bits correspondientes a mode
    for (i = 1; tr[i] != NULL; i++) {
        if (!strcmp(tr[i], "cr")) mode |= O_CREAT;
        else if (!strcmp(tr[i], "ex")) mode |= O_EXCL;
        else if (!strcmp(tr[i], "ro")) mode |= O_RDONLY;
        else if (!strcmp(tr[i], "wo")) mode |= O_WRONLY;
        else if (!strcmp(tr[i], "rw")) mode |= O_RDWR;
        else if (!strcmp(tr[i], "ap")) mode |= O_APPEND;
        else if (!strcmp(tr[i], "tr")) mode |= O_TRUNC;
        else {
            printf("Error: Modo de apertura no reconocido: %s\n", tr[i]);
            return; // Si hay un modo no reconocido, terminamos la ejecución
        }
    }

    // Intentamos abrir el archivo con las banderas acumuladas en mode
    if ((df = open(tr[0], mode, 0777)) == -1) {
        perror("Imposible abrir fichero");
    } else {
        anadirAFicherosAbiertos(df, mode, tr[0]); // Añadir a la lista de archivos abiertos
        printf("Añadida entrada a la tabla de ficheros abiertos: %s\n", tr[0]);
    }
}

void cmdClose (char *tr[]) {
    int df;

    // Si no se proporciona un descriptor, lista los archivos abiertos
    if (tr[0] == NULL || (df = atoi(tr[0])) < 0) { /*no hay parametro*/
        listarFicherosAbiertos();
        return;
    }
    if (close(df) == -1)
        perror("Imposible cerrar descriptor");
    else {
        close(df);
        eliminarDeFicherosAbiertos(df); // Eliminar el archivo de la lista de archivos abiertos
        printf("Cerrado descriptor %d\n", df);
    }
}

void cmdDup (char * tr[]){
    int df, duplicado;
    char aux[MAXNAME],*p;

    // Si no se proporciona un descriptor, lista los archivos abiertos
    if (tr[0] == NULL || (df = atoi(tr[0])) < 0) { /*no hay parametro*/
        listarFicherosAbiertos();
        return;
    }

    // Intenta duplicar el descriptor
    duplicado=dup(df);
    if(duplicado == -1) {
        perror("Imposible duplicar el descriptor");
        return;
    }

    // Obtener el nombre del archivo asociado al descriptor duplicado
    p = nombreFicheroDescriptor(df);
    sprintf (aux,"dup %d (%s)",df, p);

    // Añadir el descriptor duplicado a la lista de archivos abiertos
    anadirAFicherosAbiertos(duplicado, fcntl(duplicado,F_GETFL), aux); // fcntl pasandole F_GETFL como segundo argumento devuelve el modo de acceso del archivo
    printf("Descriptor duplicado: %d, original: %d (%s)\n", duplicado, df, p);
}

void cmdCd (char *tr[]) {
    char dir[PATH_MAX];  // Buffer para almacenar el directorio actual

    // Si se invoca sin argumentos (tr[0] es NULL), imprimir el directorio actual
    if (tr[0] == NULL) {
        if (getcwd(dir, sizeof(dir)) != NULL) {
            printf("Directorio actual: %s\n", dir);
        } else {
            perror("Error al obtener el directorio actual");
        }
    } else {
        // Si se invoca con un argumento (tr[0] no es NULL), intentar cambiar el directorio
        if (chdir(tr[0]) == 0) {
            // Cambio exitoso
            if (getcwd(dir, sizeof(dir)) != NULL) {
                printf("Nuevo directorio: %s\n", dir);
            }
        } else {
            // Error en el cambio de directorio
            perror("Error al cambiar de directorio");
        }
    }
}

void cmdHistoric(char *tr[]){
    if(tr[0] == NULL){
        listarComandos(); // Mostrar todos los comandos en la lista de comandos si no se proporcionan argumentos
    }
    else if (strchr(tr[0], '-') != NULL){
        int num = (atoi(tr[0])*-1); // Mostrar los últimos N comandos si el argumento es negativo
        listarNComandos(num);
    }
    else {
        int num = atoi(tr[0]); // Ejecutar un comando específico del historico
        if (num < 0) return;
        char *command = obtenerComando(num); // Obtener el comando del historico
        char *trozos[100];
        printf("Ejecutando hist (%d): %s", num, command);
        procesarEntrada(command, trozos); // Volver a procesar el comando
    }
}

void cmdAuthors(char *tr[]){
    const char *names = "Jorge Mera | Adrian Novio";
    const char *emails = "jorge.mera@udc.es | adrian.nvilarino@udc.es";

    if (tr[0] == NULL) {  // Si no se pasan argumentos, imprime ambos
        printf("Nombre: %s\nCorreo: %s\n", names, emails);
    } else if (strcmp(tr[0], "-l") == 0) {  // cmdAuthors -l: solo correos
        printf("Correo: %s\n", emails);
    } else if (strcmp(tr[0], "-n") == 0) {  // cmdAuthors -n: solo nombres
        printf("Nombre: %s\n", names);
    } else {
        printf("Comando no reconocido para authors. Use 'authors', 'authors -l' o 'cmdAuthors -n'.\n");
    }
}

void getPid(char * tr[]) {
    // Verificamos que el array de argumentos no esté vacío
    if (tr[0] != NULL && strcmp(tr[0], "pid") == 0) {
        printf("Pid de Shell (pid): %d\n", getpid());  // Imprime el PID del proceso actual
    }
}

void getPPid(char *tr[]) {
    if (tr[0] != NULL && strcmp(tr[0], "ppid") == 0) {
        printf("Pid del padre del shell (ppid): %d\n", getppid()); // Imprime el PPID del proceso padre de la shell
    }
}

void getDate(char *tr[]){
    if (tr[1] == NULL){
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        printf("Fecha y hora actual: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
               tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
    else if (strcmp(tr[1], "-d") == 0){
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        printf("Fecha actual: %d-%02d-%02d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    }
    else if (strcmp(tr[1], "-t") == 0){
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        printf("Hora actual: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
}

void infoSys(char *tr[]){
    struct utsname uts;
    if (uname(&uts) == -1){
        perror("Error al obtener la información del sistema");
        return;
    }
    if (tr[1] == NULL){
        printf("Sistema operativo: %s\n", uts.sysname);
        printf("Nombre del host: %s\n", uts.nodename);
        printf("Versión del sistema: %s\n", uts.release);
        printf("Versión del kernel: %s\n", uts.version);
        printf("Arquitectura del hardware: %s\n", uts.machine);
    }
}

void help(char *tr[]){
    if (tr[1] == NULL){
        printf("Comandos disponibles:\n");
        printf("authors\n");
        printf("pid\n");
        printf("ppid\n");
        printf("cd\n");
        printf("date\n");
        printf("historic\n");
        printf("open\n");
        printf("close\n");
        printf("dup\n");
        printf("infosys\n");
        printf("help\n");
        printf("quit\n");
        printf("exit\n");
        printf("bye\n");
        printf("makefile\n");
        printf("makedir\n");
        printf("listfile\n");
        printf("cwd\n");
        printf("listdir\n");
        printf("reclist\n");
        printf("revlist\n");
        printf("erase\n");
        printf("delrec\n");
    }
    else if (strcmp(tr[1], "authors") == 0){
        printf("authors [-n|-l] \tMuestra los nombres y/o logins de los autores\n"
            );
    }
    else if (strcmp(tr[1], "pid") == 0){
        printf("pid \tMuestra el pid del shell\n");
    }
    else if (strcmp(tr[1], "ppid") == 0){
        printf("ppid \tMuestra el pid del proceso padre del shell\n");
    }

    else if (strcmp(tr[1], "cd") == 0){
        printf("cd [dir] \tCambia (o muestra) el directorio actual del shell\n");
    }

    else if (strcmp(tr[1], "date") == 0) {
        printf("date [-d|-t] \tMuestra la fecha y/o la hora actual\n"
               "\t-d: imprime la fecha actual en formato DD/MM/YYYY\n"
               "\t-t: imrpime el tiempo actual en formato hh:mm:ss");
    }

    else if (strcmp(tr[1], "historic") == 0){
        printf("historic [-N|N] \tMuestra el historico de los comandos ingresados con su numero de orden\n"
               "\t-N: muestra los N últimos\n"
               "\tN: repite el comando N\n");
    }

    else if (strcmp(tr[1], "open") == 0){
        printf("open fich m1 m2...\tAbre el fichero fich\n"
               "\ty lo anade a la lista de ficheros abiertos del shell\n"
               "\tm1, m2..es el modo de apertura (or bit a bit de los siguientes)\n"
               "\tcr: O_CREAT\tap: O_APPEND\n"
               "\tex: O_EXCL \tro: O_RDONLY\n"
               "\trw: O_RDWR \two: O_WRONLY\n"
               "\ttr: O_TRUNC\n");
    }
    else if (strcmp(tr[1], "close") == 0){
        printf("close df\tCierra el descriptor df y elimina el correspondiente fichero de la lista de ficheros abiertos\n");
    }
    else if (strcmp(tr[1], "dup") == 0){
        printf("dup df\tDuplica el descriptor de fichero df y anade una nueva entrada a la lista ficheros abiertos\n");
    }

    else if (strcmp(tr[1], "infosys") == 0){
        printf("infosys \tMuestra informacion de la maquina donde corre el shell\n");
    }
    else if (strcmp(tr[1], "help") == 0){
        printf("help [cmd] \tMuestra ayuda sobre los comandos\n"
               "\tcmd: info sobre el comando cmd\n");
    }
    else if (strcmp(tr[1], "quit") == 0){
        printf("quit \tTermina la ejecucion del shell\n");
    }

    else if (strcmp(tr[1], "exit") == 0){
        printf("exit \tTermina la ejecucion del shell\n");
    }

    else if (strcmp(tr[1], "bye") == 0){
        printf("bye \tTermina la ejecucion del shell\n");
    }

    else if(strcmp(tr[1], "makefile") == 0){
        printf("makefile [name] \tCrea un fichero de nombre name\n");
    }

    else if(strcmp(tr[1], "makedir") == 0){
        printf("makedir [name] \tCrea un directorio de nombre name\n");
    }

    else if(strcmp(tr[1], "listfile") == 0){
        printf("listfile [-long][-link][-acc] name1 name2 ..	lista ficheros\n"
               "\t-long: listado largo\n"
               "\t-acc: acesstime\n"
               "\t-link: si es enlace simbolico, el path contenido\n");
    }

    else if(strcmp(tr[1], "cwd") == 0){
        printf("cwd \tMuestra el directorio actual del shell\n");
    }

    else if(strcmp(tr[1], "listdir") == 0){
        printf("listdir [-hid][-long][-link][-acc] n1 n2 ..	lista contenidos de directorios\n"
               "\t-long: listado largo\n"
               "\t-hid: incluye los ficheros ocultos\n"
               "\t-acc: acesstime\n"
               "\t-link: si es enlace simbolico, el path contenido\n");
    }

    else if(strcmp(tr[1], "reclist") == 0){
        printf("reclist [-hid][-long][-link][-acc] n1 n2 ..	lista recursivamente contenidos de directorios (subdirs despues)\n"
               "\t-hid: incluye los ficheros ocultos\n"
               "\t-long: listado largo\n"
               "\t-acc: acesstime\n"
               "\t-link: si es enlace simbolico, el path contenido\n");
    }

    else if(strcmp(tr[1], "revlist") == 0){
        printf("revlist [-hid][-long][-link][-acc] n1 n2 ..	lista recursivamente contenidos de directorios (subdirs antes)\n"
               "\t-hid: incluye los ficheros ocultos\n"
               "\t-long: listado largo\n"
               "\t-acc: acesstime\n"
               "\t-link: si es enlace simbolico, el path contenido\n");
    }

    else if (strcmp(tr[1], "erase") == 0){
        printf("erase [name1 name2 ..]\tBorra ficheros o directorios vacios\n");
    }

    else if(strcmp(tr[1], "delrec") == 0){
        printf("delrec [name1 name2 ..] \tBorra ficheros o directorios no vacios recursivamente\n");
    }

    else{
        printf("Comando no reconocido. Use 'help' para ver los comandos disponibles.\n");
    }
}

void makeFile(char *tr[]){
    if (tr[1] == NULL){
        printf("No se ha especificado el nombre del archivo\n");
        return;
    }
    int fd = open(tr[1], O_CREAT | O_EXCL |O_RDWR, 0777);
    if (fd == -1){
        if (errno == EEXIST){
            printf("El archivo ya existe'%s'\n", tr[1]);
            return;
        }
        perror("Error al crear el archivo");
        return;
    }
    anadirAFicherosAbiertos(fd, O_RDWR, tr[1]);
    printf("Archivo %s creado con éxito\n", tr[1]);

    if (close(fd) == -1) {
        perror("Error al cerrar el archivo");
        return;
    }
    eliminarDeFicherosAbiertos(fd);
}

void makeDir(char *tr[]){
    if (tr[1] == NULL){
        printf("No se ha especificado el nombre del directorio\n");
        return;
    }
    if (mkdir(tr[1], 0777) == -1){
        perror("Error al crear el directorio");
        return;
    }
    printf("Directorio %s creado con éxito\n", tr[1]);
}

void listFile(char *tr[]) {
    struct stat file_info;
    char *nombre_archivo = NULL;
    int long_format = 0, acc_time = 0, is_link = 0;


    // Recorrer el array de argumentos, si se detecta alguna opción adicional añadirla, y si no,
    // mostrar información del archivo con las opciones encontradas
    for (int i = 1; tr[i] != NULL; i++) {
        if (strcmp(tr[i], "-long") == 0) {
            long_format = 1;
        } else if (strcmp(tr[i], "-acc") == 0) {
            acc_time = 1;
        } else if (strcmp(tr[i], "-link") == 0) {
            is_link = 1;
        } else {
            nombre_archivo = tr[i];  // Si no es opción, asume que es el nombre del archivo
            listFileAux(nombre_archivo, file_info, long_format, acc_time, is_link);
        }
    }

    // Si no se proporciona un archivo, usar el directorio actual
    if (nombre_archivo == NULL) {
        nombre_archivo = getcwd(nombre_archivo, sizeof(nombre_archivo));
        listFileAux(nombre_archivo, file_info, long_format, acc_time, is_link);
    }
}

void cwd(char *tr[]) {
    char dir[PATH_MAX];  // Buffer para almacenar el directorio actual

    if (tr[1] == NULL) {
        if (getcwd(dir, sizeof(dir)) != NULL) {
            printf("Directorio actual: %s\n", dir);
        } else {
            perror("Error al obtener el directorio actual");
        }
    }
}

void listdir(char *tr[]) {
    int long_format = 0, show_hidden = 0, acc_time = 0, show_link = 0;
    char *dir_name = NULL;

    // Recorrer el array de argumentos, si se detecta alguna opción adicional añadirla, y si no,
    // mostrar información del archivo con las opciones encontradas
    for (int i = 1; tr[i] != NULL; i++) {
        if (strcmp(tr[i], "-long") == 0) {
            long_format = 1;
        } else if (strcmp(tr[i], "-hid") == 0) {
            show_hidden = 1;
        } else if (strcmp(tr[i], "-acc") == 0) {
            acc_time = 1;
        } else if (strcmp(tr[i], "-link") == 0) {
            show_link = 1;
        } else {
            dir_name = tr[i];  // Asumimos que es el nombre del directorio
            // Listar el contenido del directorio
            listDirectoryContents(dir_name, long_format, show_hidden, acc_time, show_link);
        }
    }

    // Si no se proporciona un directorio, usar el directorio actual
    if (dir_name == NULL) {
        char cwd[BUFFER_SIZE];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        }
    }
}

void reclist(char *tr[]){
    int long_format = 0, show_hidden = 0, acc_time = 0, show_link = 0;
    char *dir_name = NULL;

    // Recorrer el array de argumentos, si se detecta alguna opción adicional añadirla, y si no,
    // mostrar información del archivo con las opciones encontradas
    for (int i = 1; tr[i] != NULL; i++) {
        if (strcmp(tr[i], "-long") == 0) {
            long_format = 1;
        } else if (strcmp(tr[i], "-hid") == 0) {
            show_hidden = 1;
        } else if (strcmp(tr[i], "-acc") == 0) {
            acc_time = 1;
        } else if (strcmp(tr[i], "-link") == 0) {
            show_link = 1;
        } else {
            dir_name = tr[i];  // Asumimos que es el nombre del directorio
            // Listar el contenido del directorio
            listDirectoryContentsRecursively(dir_name, long_format, show_hidden, acc_time, show_link, true);
        }
    }

    // Si no se proporciona un directorio, mostrar el directorio actual
    if (dir_name == NULL) {
        char cwd[BUFFER_SIZE];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        }
    }
}

void revlist(char *tr[]){
    int long_format = 0, show_hidden = 0, acc_time = 0, show_link = 0;
    char *dir_name = NULL;

    // Detectar opciones y nombres de directorios
    for (int i = 1; tr[i] != NULL; i++) {
        if (strcmp(tr[i], "-long") == 0) {
            long_format = 1;
        } else if (strcmp(tr[i], "-hid") == 0) {
            show_hidden = 1;
        } else if (strcmp(tr[i], "-acc") == 0) {
            acc_time = 1;
        } else if (strcmp(tr[i], "-link") == 0) {
            show_link = 1;
        } else {
            dir_name = tr[i];  // Asumimos que es el nombre del directorio
            // Listar el contenido del directorio
            listDirectoryContentsRecursively(dir_name, long_format, show_hidden, acc_time, show_link, false);
        }
    }

    // Si no se proporciona un directorio, mostrar el directorio actual
    if (dir_name == NULL) {
        char cwd[BUFFER_SIZE];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        }
    }
}

// Función que borra un fichero o directorio vacío
void erase(char *tr[]) {
    int i = 1;
    char dir[PATH_MAX];  // Buffer para almacenar el directorio actual

    // Si no se pasa ningún archivo o directorio, mostrar el directorio actual
    if (tr[1] == NULL) {
        if (getcwd(dir, sizeof(dir)) != NULL) {
            printf("Directorio actual: %s\n", dir);
        } else {
            perror("Error al obtener el directorio actual\n");
        }
        return;
    }

    // Recorrer los argumentos para borrar múltiples archivos/directorios
    while (tr[i] != NULL) {
        deleteFileOrEmptyDirectory(tr[i]);
        // Pasar al siguiente argumento
        i++;
    }
}

// Función que borra ficheros o directorios no vacíos recursivamente
void delrec(char *tr[]) {
    char *dir_name = NULL;

    // Detectar opciones y nombres de directorios
    for (int i = 1; tr[i] != NULL; i++) {
        dir_name = tr[i];  // Asumimos que es el nombre del directorio
        // Listar el contenido del directorio
        deleteDirectoryContentsRecursively(dir_name);
        deleteFileOrEmptyDirectory(dir_name);
    }

    // Si no se proporciona un directorio, mostrar el directorio actual
    if (dir_name == NULL) {
        char cwd[BUFFER_SIZE];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        }
    }
}
