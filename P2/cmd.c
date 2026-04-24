#include "processCommand.h"
#include "auxfunctions.h"

void cmdOpen(char *tr[]) {
    int df, mode = 0;

    if (tr[0] == NULL) { // No hay parámetro, listar archivos abiertos
        listarFicherosAbiertos();
        return;
    }

    // Recorremos los parámetros después del nombre del archivo (a partir de tr[1])
    // añadiendo los bits correspondientes a mode
    for (int i = 1; tr[i] != NULL; i++) {
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
    int df;
    char aux[MAXNAME];

    // Si no se proporciona un descriptor, lista los archivos abiertos
    if (tr[0] == NULL || (df = atoi(tr[0])) < 0) { /*no hay parametro*/
        listarFicherosAbiertos();
        return;
    }

    // Intenta duplicar el descriptor
    int duplicado = dup(df);
    if(duplicado == -1) {
        perror("Imposible duplicar el descriptor");
        return;
    }

    // Obtener el nombre del archivo asociado al descriptor duplicado
    char* p = nombreFicheroDescriptor(df);
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
    int num_comandos = obtenerNumeroComandos();  // Obtener el número total de comandos en el historial

    if(tr[0] == NULL){
        listarComandos(); // Mostrar todos los comandos en la lista de comandos si no se proporcionan argumentos
    }
    else if (strchr(tr[0], '-') != NULL){
        int num = (atoi(tr[0])*-1); // Mostrar los últimos N comandos si el argumento es negativo
        listarNComandos(num);
    }
    else {
        int num = atoi(tr[0]); // Ejecutar un comando específico del historico
        if (num < 0 || num >= num_comandos) {
            printf("Error: El comando %d no existe en el historial. Solo hay %d comandos.\n", num, num_comandos);
            return;
        }
        char *command = obtenerComando(num); // Obtener el comando del historico
        if (command == NULL){
            printf("Error: Comando %d no encontrado en el historial.\n", num);
            return;
        }
        char *trozos[100];
        printf("Ejecutando hist (%d): %s", num, command);
        procesarEntrada(command, trozos, environ); // Volver a procesar el comando
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
        printf("authors\n"); printf("pid\n");
        printf("ppid\n"); printf("cd\n");
        printf("date\n"); printf("historic\n");
        printf("open\n"); printf("close\n");
        printf("dup\n"); printf("infosys\n");
        printf("help\n"); printf("quit\n");
        printf("exit\n"); printf("bye\n");
        printf("makefile\n"); printf("makedir\n");
        printf("listfile\n"); printf("cwd\n");
        printf("listdir\n"); printf("reclist\n");
        printf("revlist\n"); printf("erase\n");
        printf("delrec\n"); printf("allocate\n");
        printf("deallocate\n"); printf("memfill\n");
        printf("memdump\n"); printf("memory\n");
        printf("readfile\n"); printf("writefile\n");
        printf("read\n"); printf("write\n");
        printf("recurse\n"); printf("getuid\n");
        printf("setuid\n"); printf("showvar\n");
        printf("changevar\n"); printf("subsvar\n");
        printf("fork\n"); printf("search\n");
        printf("exec\n"); printf("execpri\n");
        printf("fg\n"); printf("fgpri\n");
        printf("back\n"); printf("backpri\n");
        printf("listjobs\n"); printf("deljobs\n");

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

    else if(strcmp(tr[1], "allocate") == 0){
        printf("allocate [-malloc|-shared|-createshared|-mmap]... Asigna un bloque de memoria\n"
                "\t-malloc tam: asigna un bloque malloc de tamano tam\n"
                "\t-createshared cl tam: asigna (creando) el bloque de memoria compartida de clave cl y tamano tam\n"
                "\t-shared cl: asigna el bloque de memoria compartida (ya existente) de clave cl\n"
                "\t-mmap fich perm: mapea el fichero fich, perm son los permisos\n");
    }

    else if(strcmp(tr[1], "memory") == 0){
        printf("memory [-blocks|-funcs|-vars|-all|-pmap] ..\tMuestra detalles de la memoria del proceso\n"
                "\t-blocks: los bloques de memoria asignados\n"
                "\t-funcs: las direcciones de las funciones\n"
                "\t-vars: las direcciones de las variables\n"
                "\t-all: todo\n"
                "\t-pmap: muestra la salida del comando pmap(o similar)\n");
    }

    else if(strcmp(tr[1], "deallocate") == 0){
        printf("deallocate [-malloc|-shared|-delkey|-mmap|addr]..\tDesasigna un bloque de memoria\n"
               "\t-malloc tam: desasigna el bloque malloc de tamano tam\n"
               "\t-shared cl: desasigna (desmapea) el bloque de memoria compartida de clave cl\n"
               "\t-delkey cl: elimina del sistema (sin desmapear) la clave de memoria cl\n"
               "\t-mmap fich: desmapea el fichero mapeado fich\n"
               "\taddr: desasigna el bloque de memoria en la direccion addr\n");
    }

    else if(strcmp(tr[1], "memfill") == 0){
        printf("memfill addr cont byte \tLlena la memoria a partir de addr con byte\n");
    }

    else if(strcmp(tr[1], "memdump") == 0){
        printf("memdump addr cont \tVuelca en pantallas los contenidos (cont bytes) de la posicion de memoria addr\n");
    }

    else if(strcmp(tr[1], "readfile") == 0){
        printf("readfile fiche addr cont \tLee cont bytes desde fich a la direccion addr\n");
    }

    else if(strcmp(tr[1], "writefile") == 0){
        printf("writefile fiche addr cont \tEscribe cont bytes desde la direccion addr a fich\n");
    }

    else if(strcmp(tr[1], "read") == 0){
        printf("read df addr cont\tTransfiere cont bytes del fichero descrito por df a la dirección addr\n");
    }

    else if(strcmp(tr[1], "write") == 0){
        printf("write df addr cont\tTransfiere cont bytes desde la dirección addr al fichero descrito por df\n");
    }

    else if(strcmp(tr[1], "recurse") == 0){
        printf("recurse [n]\tInvoca a la funcion recursiva n veces\n");
    }

    else if(strcmp(tr[1], "getuid") == 0){
        printf("getuid\tMuestra las credenciales del proceso que ejecuta el shell\n");
    }

    else if(strcmp(tr[1], "setuid") == 0){
        printf("setuid [-l] id\tCambia las credenciales del proceso que ejecuta el shell\n"
                "\tid: establece la credencial al valor numerico id\n"
                "\t-l id: establece la credencial a login id\n");
    }

    else if(strcmp(tr[1], "showvar") == 0){
        printf("showvar v1 v2 ...\tMuestra el valor y las direcciones de la variable de entorno v\n");
    }

    else if(strcmp(tr[1], "changevar") == 0){
        printf("changevar [-a|-e|-p] var valor\tCambia el valor de una variable de entorno\n"
                "\t-a: accede por el tercer arg de main\n"
                "\t-e: accede mediante environ\n"
                "\t-p: accede mediante putenv\n");
    }

    else if(strcmp(tr[1], "subsvar") == 0)
    {
        printf("subsvar [-a|-e] var1 var2 valor\tSustituye la variable de entorno var1 con var2=valor\n"
                "\t-a: accede por el tercer arg de main\n"
                "\t-e: accede mediante environ\n");
    }

    else if(strcmp(tr[1], "environ") == 0){
        printf("environ [-environ|-addr]\tMuestra el entorno del proceso"
                "\t-environ: accede usando environ (en lugar del tercer arg de main)\n"
                "\t-addr: muestra el valor y donde se almacenan environ y el 3er arg main\n");
    }

    else if(strcmp(tr[1], "fork") == 0){
        printf("fork\tEl shell hace fork y queda en espera a que su hijo termine\n");
    }

    else if(strcmp(tr[1], "search") == 0){
        printf("search [-add|-del|-clear|-path]\tManipula o muestra la ruta de busqueda del shell (path)\n"
                "\t-add dir: aniade dir a la ruta de busqueda(equiv +dir)\n"
                "\t-del dir: elimina dir de la ruta de busqueda (equiv -dir)\n"
                "\t-clear: vacia la ruta de busqueda\n"
                "\t-path: importa el PATH en la ruta de busqueda\n");
    }

    else if(strcmp(tr[1], "exec") == 0){
        printf("exec VAR1 VAR2 ..prog args....[@pri]\tEjecuta, sin crear proceso,prog con argumentos\n"
                "\ten un entorno que contiene solo las variables VAR1, VAR2...\n");
    }

    else if(strcmp(tr[1], "execpri") == 0){
        printf("execpri prio prog args....	Ejecuta, sin crear proceso, "
               "prog con argumentos con la prioridad cambiada a prio");
    }

    else if(strcmp(tr[1], "fg") == 0){
        printf("fg prog args...\tCrea un proceso que ejecuta en primer plano prog con argumentos\n");
    }

    else if(strcmp(tr[1], "fgpri") == 0){
        printf("fgpri prio prog args...\tCrea un proceso que ejecuta en primer plano prog con argumentos "
               "con la prioridad cambiada a prio\n");
    }

    else if(strcmp(tr[1], "back") == 0){
        printf("back prog args...\tCrea un proceso que ejecuta en segundo plano prog con argumentos\n");
    }

    else if(strcmp(tr[1], "backpri") == 0){
        printf("backpri prio prog args...\tCrea un proceso que ejecuta en segundo plano prog con argumentos "
               "con la prioridad cambiada a prio\n");
    }

    else if(strcmp(tr[1], "listjobs") == 0){
        printf("listjobs\tLista los procesos en segundo plano\n");
    }

    else if(strcmp(tr[1], "deljobs") == 0){
        printf("deljobs [-term][-sig]\tElimina los procesos de la lista procesos en sp\n"
                "\t-term: los terminados\n"
                "\t-sig: los terminados por senal\n");
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
    const int fd = open(tr[1], O_CREAT | O_EXCL |O_RDWR, 0777);
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
    if (tr[1] == NULL)
    {
        char dir[PATH_MAX];
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

    // Si no se pasa ningún archivo o directorio, mostrar el directorio actual
    if (tr[1] == NULL) {
        char dir[PATH_MAX];
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

/**********************************************************************************************************************/

void do_AllocateCreateshared (char *tr[])
{
    void *p;

    // Si faltan argumentos, se imprime la lista de memoria
    if (tr[0]==NULL || tr[1]==NULL) {
        imprimirListaMemoria();
        return;
    }

    // Convertir el primer argumento en clave y el segundo en tamaño
    key_t cl = (key_t)strtoul(tr[0],NULL, 10);
    size_t tam = (size_t)strtoul(tr[1],NULL, 10);
    // No se permite asignar bloques de 0 bytes
    if (tam==0) {
        printf ("No se asignan bloques de 0 bytes\n");
        return;
    }
    // Intentar obtener memoria compartida
    if ((p=ObtenerMemoriaShmget(cl,tam))!=NULL)
        // Si se asignó correctamente, imprimir el tamaño y dirección
        printf ("Asignados %lu bytes en %p\n",(unsigned long) tam, p);
    else
        // Si no se pudo asignar, se muestra un error
        printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) cl,strerror(errno));
}

void do_AllocateMmap(char *tr[])
{
    char *perm;
    void *p;
    int protection=0;

    // Si no se proporciona archivo, listar bloques asignados
    if (tr[0]==NULL)
    {imprimirListaMemoria(); return;}

    if ((perm=tr[1])!=NULL && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }
    // Si no hay permisos válidos, emitir un mensaje de error
    if(protection == 0){
        printf("No se han introducido permisos\n");
        return;
    }
    // Intentar mapear el archivo a memoria
    if ((p=MapearFichero(tr[0],protection))==NULL)
        perror ("Imposible mapear fichero\n");
    else
        printf ("fichero %s mapeado en %p\n", tr[0], p);
}

void do_AllocateMalloc(char *tr[]) {
    if (tr[0]==NULL) {
        imprimirListaMemoria();
        return;
    }
    // Convertir el tamaño del bloque de memoria
    int size = (int) strtol(tr[0], NULL, 10);
    if (errno == ERANGE || size > INT_MAX || size < INT_MIN) {
        // Si el valor está fuera del rango de int
        printf("Error: el valor está fuera del rango de int.\n");
        return;
    }
    void *address = add_malloc_block(size);  // Capturamos la dirección del bloque
    printf("Asignados %d bytes en %p\n", size, address);  // Mostramos el mensaje con los detalles
}

void do_AllocateShared(char *tr[]) {
    void *p;

    if (tr[0] == NULL) {
        imprimirListaMemoria();
        return;
    }

    key_t cl = (key_t)strtoul(tr[0], NULL, 10);

    // Intentar asociar el bloque de memoria compartida ya existente
    if ((p = ObtenerMemoriaShmget(cl, 0)) != NULL) {
        printf("Memoria compartida existente con clave %lu asignada en %p\n", (unsigned long) cl, p);
    } else {
        printf("Imposible asignar memoria compartida clave %lu: %s\n", (unsigned long) cl, strerror(errno));
    }
}

void do_Allocate(char *tr[]) {
    if (tr[0] == NULL) {
        imprimirListaMemoria(); // Imprimir todos los bloques si no se especifica un parámetro
        return;
    }
    if (strcmp(tr[0], "-malloc") == 0) {
        if (tr[1] == NULL) {
            imprimirlistaMemoriaPorTipo(MALLOC); // Imprimir bloques malloc si no hay tamaño
        } else {
            do_AllocateMalloc(&tr[1]); // Asignar memoria malloc
        }
    } else if (strcmp(tr[0], "-mmap") == 0) {
        if (tr[1] == NULL) {
            imprimirlistaMemoriaPorTipo(MMAP); // Imprimir bloques mmap si no hay archivo
        } else {
            do_AllocateMmap(&tr[1]); // Asignar memoria mmap
        }
    } else if (strcmp(tr[0], "-createshared") == 0) {
        if (tr[1] == NULL) {
            imprimirlistaMemoriaPorTipo(SHARED); // Imprimir bloques shared si no hay clave y tamaño
        } else {
            do_AllocateCreateshared(&tr[1]); // Asignar memoria compartida
        }
    } else if (strcmp(tr[0], "-shared") == 0) {
        if (tr[1] == NULL) {
            imprimirlistaMemoriaPorTipo(SHARED); // Imprimir bloques shared si no hay clave
        } else {
            do_AllocateShared(&tr[1]); // Adjuntar memoria compartida
        }
    } else {
        printf("uso: allocate [-malloc|-shared|-createshared|-mmap] ....\n");
    }
}

void do_DeallocateShared(char *tr[]) {
    if (tr[0] == NULL) {
        printf("Error: Falta clave para desasignar memoria compartida\n");
        return;
    }
    char *key=tr[0];
    key_t clave;
    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("      shared necesita una clave valida\n");
        return;
    }
    detachSharedBlock(clave);
}

void do_DeallocateDelkey (char *tr[])
{
    key_t clave;
    int id;
    const char *key=tr[0];

    // Verificar si se proporcionó una clave válida
    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("delkey necesita una clave valida\n");
        return;
    }
    // Obtener el id del segmento de memoria compartida asociado a la clave
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
    }
    // Marcar el segmento para eliminación
    if (shmctl(id,IPC_RMID,NULL)==-1) {
        perror ("shmctl: imposible eliminar memoria compartida\n");
    } else {
        printf("Memoria compartida de clave %d eliminada del sistema\n", clave);
    }
}

void do_DeallocateMalloc (char *tr[]) {
    if (tr[0] == NULL) {
        printf("Error: Falta el tamano del bloque malloc a liberar\n");
        return;
    }
    // Convertir el tamaño del bloque malloc
    const int size = (int) strtoul(tr[0], NULL, 10);
    // Liberar el bloque malloc por su tamaño
    freeMallocBlockBySize(size);
}

void do_DeallocateMmap (char *tr[]) {
    if (tr[0] == NULL) {
        printf("Error: Falta el nombre del archivo para liberar\n");
        return;
    }
    // Desasignar memoria mapeada identificada por el nombre del archivo
    deallocateMmap(tr[0]);
}

void do_DeallocateAddr(char *tr[]) {
    if (tr[0] == NULL) {
        imprimirListaMemoria(); // Imprimir todos los bloques si no se especifica un parámetro
        return;
    }

    // Convertir dirección proporcionada a puntero
    char *endptr = NULL;
    unsigned long addr_val = strtoul(tr[0], &endptr, 16); // Interpretar como número hexadecimal

    // Si no se pudo convertir completamente o contiene caracteres no válidos
    if (*endptr != '\0') {
        printf("Dirección no válida: %s\n", tr[0]);
        return;
    }

    // Convertir a puntero
    void *addr = (void *)addr_val;

    // Intentar desasignar la memoria directamente
    printf("Intentando desasignar dirección: 0x%lx\n", addr_val);
    deallocateAddress(addr);
}

void do_Deallocate(char *tr[]) {
    if (tr[0] == NULL) {
        imprimirListaMemoria(); // Imprimir todos los bloques si no se especifica un parámetro
        return;
    }
    if (strcmp(tr[0], "-malloc") == 0) {
        if (tr[1] == NULL) {
            imprimirlistaMemoriaPorTipo(MALLOC); // Imprimir bloques malloc si no hay tamaño
        } else {
            do_DeallocateMalloc(&tr[1]); // Deasignar memoria malloc
        }
    } else if (strcmp(tr[0], "-mmap") == 0) {
        if (tr[1] == NULL) {
            imprimirlistaMemoriaPorTipo(MMAP); // Imprimir bloques mmap si no hay archivo
        } else {
            do_DeallocateMmap(&tr[1]); // Desasignar memoria mmap
        }
    } else if (strcmp(tr[0], "-shared") == 0) {
        if (tr[1] == NULL) {
            imprimirlistaMemoriaPorTipo(SHARED); // Imprimir bloques shared si no hay clave
        } else {
            do_DeallocateShared(&tr[1]); // Adjuntar memoria compartida
        }
    } else if (strcmp(tr[0], "-delkey") == 0)
    {
        if (tr[1] == NULL) {
            printf("delkey necesita clave_valida\n");
        } else {
            do_DeallocateDelkey(&tr[1]); // Eliminar clave de memoria compartida
        }
    }else {
        do_DeallocateAddr(tr);
    } // Eliminar memoria por dirección
}

void Cmd_MemFill(char *tr[]) {
    void *p;
    size_t cont;
    unsigned char byte_val;

    // Verificar que se proporcionen todos los parámetros
    if (tr[0] == NULL || tr[1] == NULL || tr[2] == NULL) {
        printf("Uso: memfill addr cont ch\n");
        return;
    }

    // Convertir la dirección de memoria
    p = cadtop(tr[0]);
    if (p == NULL) {
        printf("Dirección de memoria no válida: %s\n", tr[0]);
        return;
    }

    // Convertir la cantidad de bytes
    cont = (size_t)atoll(tr[1]);
    if (cont <= 0) {
        printf("Cantidad de bytes no válida: %s\n", tr[1]);
        return;
    }

    // Obtener el valor del byte
    if (strlen(tr[2]) == 1) {
        byte_val = tr[2][0];
    } else {
        // Intentar interpretar como número (útil para valores no imprimibles)
        long valor = strtol(tr[2], NULL, 0);
        byte_val = (unsigned char)valor;
    }

    // Rellenar la memoria
    LlenarMemoria(p, cont, byte_val);

    // Nuevo formato de salida
    printf("Llenando %zu bytes de memoria con el byte %c(%02X) a partir de la direccion %p\n",
           cont, isprint(byte_val) ? byte_val : ' ', byte_val, p);
}

void Cmd_MemDump(char *tr[]) {
    void *p;
    size_t cont;
    unsigned char *ptr;

    // Verificar que se proporcionen todos los parámetros
    if (tr[0] == NULL || tr[1] == NULL) {
        printf("Uso: memdump addr cont\n");
        return;
    }

    // Convertir la dirección de memoria
    p = cadtop(tr[0]);
    if (p == NULL) {
        printf("Dirección de memoria no válida: %s\n", tr[0]);
        return;
    }

    // Convertir la cantidad de bytes
    cont = (size_t)atoll(tr[1]);
    if (cont <= 0) {
        printf("Cantidad de bytes no válida: %s\n", tr[1]);
        return;
    }

    // Convertir a puntero a unsigned char para facilitar el análisis byte a byte
    ptr = (unsigned char *)p;

    // Imprimir dirección inicial
    printf("Volcando %zu bytes a partir de %p\n", cont, p);

    size_t i = 0;
    while (i < cont) {
        // Línea de caracteres
        for (size_t j = 0; j < 25 && i + j < cont; j++) {
            if (i + j < cont) { // Si está dentro del rango del bloque
                if (isprint(ptr[i + j])) {
                    printf("%c  ", ptr[i + j]); // Carácter imprimible
                } else {
                    printf("   "); // Espacio si no es imprimible
                }
            } else {
                printf(" "); // Espacios si el bloque termina antes de los 25 caracteres
            }
        }
        printf("\n");

        // Línea de bytes en hexadecimal
        for (size_t j = 0; j < 25; j++) {
            if (i + j < cont) { // Si está dentro del rango del bloque
                printf("%02x ", ptr[i + j]); // Representación hexadecimal
            } else {
                printf(" "); // Espacios para mantener la alineación
            }
        }
        printf("\n");

        i += 25; // Avanzar al siguiente bloque de 25 bytes
    }

    // Añadir salto de línea final
    printf("\n");
}

void Cmd_Memory(char *tr[]) {
    if (tr[0] == NULL || strcmp(tr[0], "-all") == 0) {
        imprimirVars();   // Imprimir direcciones de variables
        imprimirFuncs();  // Imprimir direcciones de funciones
        imprimirListaMemoria();  // Imprimir bloques asignados
    } else if (strcmp(tr[0], "-blocks") == 0) {
        imprimirListaMemoria();  // Imprimir bloques asignados
    } else if (strcmp(tr[0], "-funcs") == 0) {
        imprimirFuncs();  // Imprimir direcciones de funciones
    } else if (strcmp(tr[0], "-vars") == 0) {
        imprimirVars();   // Imprimir direcciones de variables
    } else if (strcmp(tr[0], "-pmap") == 0) {
        Do_pmap();  // Ejecutar pmap
    } else {
        printf("Error: Opción no reconocida para el comando memory\n");
    }
}

void recurse(char *tr[]) {
    if (tr[0] == NULL) {
        printf("Error: Falta el parámetro n para ejecutar la recursividad\n");
        return;
    }

    // Convertir el argumento a un entero
    char *endptr = NULL;
    int n = strtol(tr[0], &endptr, 10);

    // Validar que el parámetro es un número entero válido
    if (*endptr != '\0' || n < 0) {
        printf("Error: El parámetro n debe ser un número entero no negativo\n");
        return;
    }
    // Llamar a la función recursiva con el valor proporcionado
    Recursiva(n);
}

void Cmd_ReadFile(char *tr[]) {
    void *p = NULL;
    size_t cont = -1;
    ssize_t n;

    if (tr[0] == NULL || tr[1] == NULL) {
        printf("faltan parametros\n");
        return;
    }

    // Convertimos la cadena a puntero
    p = cadtop(tr[1]);

    if(p == NULL){
        printf("Direccion de memoria no valida");
        return;
    }
    // Convertir número de bytes a leer
    if (tr[2] != NULL) {
        cont = (size_t)atoll(tr[2]);
    } else {
      printf("Es necesario especificar un numero de bytes");
    }

    // Llamada a la función para leer el archivo
    n = LeerFichero(tr[0], p, cont);
    if (n == -1) {
        perror("Imposible leer fichero");
    } else {
        printf("Leidos %lld bytes de %s en %p\n", (long long)n, tr[0], p);
    }
}

void Cmd_WriteFile(char *tr[]) {
    void *p;
    size_t cont;
    ssize_t n;
    //void *temp_buffer = NULL;

    if (tr[0] == NULL || tr[1] == NULL || tr[2] == NULL) {
        printf("Uso: writefile file addr cont\n");
        return;
    }

    // Convertir la dirección o cargar el archivo fuente
    p = cadtop(tr[1]);
    if (p == NULL) {
        printf("Direccion de memoria no valida\n");
        return;
    }

    // Convertir cantidad de bytes
    cont = (size_t)atoll(tr[2]);
    if (cont <= 0) {
        // if (temp_buffer) free(temp_buffer);
        printf("Cantidad de bytes no válida: %s\n", tr[2]);
        return;
    }
    // Verificar si el archivo ya existe
    if(access(tr[0], F_OK) != -1){
        printf("Imposible escribir fichero: File exists\n");
    } else {
        // Escribir al archivo
        n = EscribirFichero(tr[0], p, cont);
        if (n == -1) {
            perror("Imposible escribir en el fichero");
        } else {
            printf("escritos %lld bytes en %s desde %p\n", (long long)n, tr[0], p);
        }
    }
}

void Cmd_Read(char *tr[]) {
    void *addr;
    size_t cont;
    int df;
    ssize_t bytesLeidos;

    // Verificar que tenemos todos los argumentos necesarios
    if (tr[0] == NULL || tr[1] == NULL || tr[2] == NULL) {
        printf("Uso: read df addr cont\n");
        return;
    }

    // Convertir argumentos
    df = atoi(tr[0]);
    addr = cadtop(tr[1]);  // Convertir dirección hexadecimal

    if(addr == NULL){
        printf("Direccion de memoria no valida");
        return;
    }

    cont = (size_t)atol(tr[2]);

    // Intentar leer del descriptor de fichero
    if ((bytesLeidos = read(df, addr, cont)) == -1) {
        perror("Error al leer del descriptor de fichero");
        return;
    }

    printf("Leídos %zd bytes del descriptor %d en %p\n", bytesLeidos, df, addr);
}

void Cmd_Write(char *tr[]) {
    void *addr;
    size_t cont;
    ssize_t n = 0;
    int df;

    // Verificar que tenemos todos los argumentos necesarios
    if (tr[0] == NULL || tr[1] == NULL || tr[2] == NULL) {
        printf("Uso: write df addr cont\n");
        return;
    }

    // Convertir argumentos
    df = atoi(tr[0]); // Descriptor de archivo
    addr = cadtop(tr[1]);  // Convertir dirección hexadecimal

    if(addr == NULL){
        printf("Direccion de memoria no valida");
        return;
    }

    cont = (size_t)atol(tr[2]); // Cantidad de bytes a escribir

    // Escribir desde la dirección de memoria al descriptor de archivo
    if ((n = write(df, addr, cont)) == -1){
        // Manejo de errores en la operación de escritura
        printf("Error al escribir en el descriptor %d desde la direccion %p:", df, addr);
        perror("");
        printf("\n");
        return;
    }

    printf("Escritos %zd bytes de la direccion %p en el descriptor %d\n", n, addr, df);
}

/**********************************************************************************************************************/

void Cmd_GetUid(char *tr[]) {
    if (tr[0] != NULL && strcmp(tr[0], "getuid") == 0)
        getUidAux();
}


void Cmd_SetUid(char *tr[]) {
    if (tr[0] == NULL) {
        getUidAux(); // Si no hay parámetros, mostrar las credenciales actuales
        return;
    }

    if (strcmp(tr[0], "-l") == 0) { // Verificar si se usa la opción -l
        if (tr[1] == NULL) {
            printf("Uso: setuid [-l] id\n");
            return;
        }

        // Buscar el usuario por nombre (login)
        struct passwd *pwd = getpwnam(tr[1]);
        if (pwd == NULL) {
            printf("Usuario no existente %s\n", tr[1]);
            return;
        }

        // Cambiar la credencial efectiva del proceso
        if (setuid(pwd->pw_uid) == -1) {
            perror("Imposible cambiar credencial");
        } else {
            printf("Credencial efectiva cambiada a %d (%s)\n", pwd->pw_uid, tr[1]);
        }
    } else {
        // Convertir el ID proporcionado en un entero
        char *endptr;
        unsigned long id = strtoul(tr[0], &endptr, 10);

        // Validar si el ID es un número
        if (*endptr != '\0') {
            printf("Imposible cambiar credencial: Operation not permitted\n");
            return;
        }

        // Validar si el ID está dentro del rango de uid_t
        if (id > UINT_MAX) {
            printf("Valor no valido de la credencial %s\n", tr[0]);
            return;
        }

        uid_t uid = (uid_t)id;

        // Intentar cambiar la credencial efectiva del proceso
        if (setuid(uid) == -1) {
            perror("Imposible cambiar credencial");
        } else {
            struct passwd *pwd = getpwuid(uid); // Buscar información del usuario por UID
            if (pwd != NULL) {
                printf("Credencial efectiva cambiada a %d (%s)\n", uid, pwd->pw_name);
            } else {
                printf("Credencial efectiva cambiada a %d (desconocido)\n", uid);
            }
        }
    }
}

void Cmd_ShowVar(char *vars[], char *envp[]) {
    // Si no se pasan argumentos, mostrar todas las variables usando envp
    if (vars[0] == NULL) {
        mostrarVars(envp);
        return;
    }

    // Iterar sobre las variables proporcionadas como argumento
    for (int i = 0; vars[i] != NULL; i++) {
        char *key = vars[i];
        char **envp_addr = NULL;
        char **environ_addr = NULL;

        // Buscar en envp, environ y getenv
        char *value_envp = buscarEnEnvp(key, envp, &envp_addr);
        char *value_environ = buscarEnEnviron(key, &environ_addr);
        char *value_getenv = getenv(key);

        // Imprimir los resultados
        printf("%s:\n", key);
        imprimirShowvar(key, value_envp, envp_addr, value_environ, environ_addr, value_getenv);
    }
}

void Cmd_ChangeVar(char *tr[], char *envp[]) {
    // Verificar que los argumentos son válidos
    if (tr[0] == NULL || tr[1] == NULL || tr[2] == NULL) {
        printf("Uso: changevar [-a|-e|-p] var valor\n");
        return;
    }

    char *opcion = tr[0]; // Por ejemplo, "-a", "-e" o "-p"
    char *var = tr[1];    // Nombre de la variable de entorno
    char *val = tr[2];    // Valor a establecer

    if (strcmp(opcion, "-a") == 0) { // Acceso mediante el tercer argumento de main
        char **env_addr = NULL;
        if (buscarEnEnvp(var, envp, &env_addr) != NULL) {
            if (cambiarVariable(var, val, env_addr) == 0) {
                printf("Variable %s cambiada a %s\n", var, val);
            }
        } else {
            manejarErrorCambio(var);
        }

    } else if (strcmp(opcion, "-e") == 0) { // Acceso mediante environ
        char **env_addr = NULL;
        if (buscarEnEnviron(var, &env_addr) != NULL) {
            if (cambiarVariable(var, val, env_addr) == 0) {
                printf("Variable %s cambiada a %s\n", var, val);
            }
        } else {
            manejarErrorCambio(var);
        }

    } else if (strcmp(opcion, "-p") == 0) { // Usar putenv
        char *new_entry = NULL;
        if (cambiarVariable(var, val, &new_entry) == -1) {
            perror("No se pudo asignar la variable");
            return;
        }
        if (putenv(new_entry) == 0) {
            printf("Variable %s cambiada a %s\n", var, val);
            // putenv toma la propiedad del string, no se debe liberar
        } else {
            perror("putenv falló");
        }
        //free(new_entry); // Liberar la cadena para la nueva variable
    } else {
        printf("Opción no válida %s. Uso: changevar [-a|-e|-p] var valor\n", opcion);
    }
}

// Función principal para el comando subsvar
void Cmd_SubsVar(char *tr[], char *envp[]) {
    // Validación de argumentos
    if (tr[0] == NULL || tr[1] == NULL || tr[2] == NULL || tr[3] == NULL) {
        printf("Uso: subsvar [-a|-e] var1 var2 valor\n");
        return;
    }

    char *opcion = tr[0];  // Opción (-a o -e)
    char *var1 = tr[1];    // Variable original
    char *var2 = tr[2];    // Nueva variable
    char *valor = tr[3];   // Nuevo valor
    char **var1_addr = NULL; // Dirección de var1 en el entorno

    if (strcmp(opcion, "-a") == 0) {
        // Acceso a través de `envp`
        if (buscarEnEnvp(var1, envp, &var1_addr) != NULL) {
            // Reemplazar la variable
            if (cambiarVariable(var2, valor, var1_addr) == 0) {
                printf("Variable %s sustituida por %s=%s\n", var1, var2, valor);
            }
        } else {
            printf("Imposible sustituir variable %s por %s: %s\n", var1, var2, strerror(ENOENT));
        }
    } else if (strcmp(opcion, "-e") == 0) {
        // Acceso a través de `environ`
        if (buscarEnEnviron(var1, &var1_addr) != NULL) {
            // Reemplazar la variable
            if (cambiarVariable(var2, valor, var1_addr) == 0) {
                printf("Variable %s sustituida por %s=%s\n", var1, var2, valor);
            }
        } else {
            printf("Imposible sustituir variable %s por %s: %s\n", var1, var2, strerror(ENOENT));
        }
    } else {
        printf("Uso: subsvar [-a|-e] var1 var2 valor\n"); // Opción inválida
    }
}

void Cmd_Environ(char *tr[], char *envp[]) {
    // Sin argumentos, se muestra el entorno con envp (como showvar sin parámetros)
    if (tr[0] == NULL) {
        mostrarVarsConEnviron(envp, "main arg3");
        return;
    }

    // Si hay un argumento, validarlo
    if (tr[1] != NULL) {
        printf("Uso: environ [-environ|-addr]\n");
        return;
    }

    if (strcmp(tr[0], "-environ") == 0) {
        // Mostrar entorno usando environ
        mostrarVarsConEnviron(environ, "environ");
    } else if (strcmp(tr[0], "-addr") == 0) {
        // Mostrar direcciones de almacenamiento
        mostrarDireccionesEnviron(envp);
    } else {
        printf("Uso: environ [-environ|-addr]\n");
    }
}

void Cmd_fork() {
    pid_t pid;

    if ((pid = fork()) == -1) {
        perror("Fallo en el fork");
        return;
    }

    // Crear un nuevo proceso usando fork
    if (pid == 0) {
        // Código del proceso hijo
        printf("ejecutando proceso %d\n", getpid());
        return; // Evita salir del proceso hijo
    }

    if (pid > 0) {
        // Código del proceso padre
        waitpid(pid, NULL, 0); // Esperar que el proceso hijo termine
    }
    else {
        // Manejo de errores en fork
        perror("Error al crear el proceso");
    }
}

void Cmd_Search(char *tr[]) {
    if (tr[0] == NULL) { // Mostrar la lista
        SearchListPrint();
        return;
    }

    if (strcmp(tr[0], "-add") == 0 && tr[1] != NULL) {
        SearchListAdd(tr[1]);
        printf("Directorio %s añadido a la lista de búsqueda\n", tr[1]);
    } else if (strcmp(tr[0], "-del") == 0 && tr[1] != NULL) {
        SearchListRemove(tr[1]);
    } else if (strcmp(tr[0], "-clear") == 0) {
        SearchListClear();
    } else if (strcmp(tr[0], "-path") == 0) {
        SearchListImportFromPath();
    } else {
        printf("Uso: search [-add|-del|-clear|-path]\n");
    }
}

void Cmd_Exec(char *tr[]) {
    if (tr[0] == NULL) {
        errno = EFAULT; // Establecer un error genérico
        perror("Imposible ejecutar");
        return;
    }

    char **new_env = NULL; // Entorno reducido
    int env_count = 0;

    // Contar variables de entorno al inicio
    while (tr[env_count] != NULL && buscarEnEnviron(tr[env_count], NULL) != NULL) {
        env_count++;
    }

    // Crear el nuevo entorno si es necesario
    if (env_count > 0) {
        new_env = malloc((env_count + 1) * sizeof(char *));
        if (new_env == NULL) {
            perror("Error al asignar memoria para el entorno");
            return;
        }
        for (int i = 0; i < env_count; i++) {
            if (cambiarVariable(tr[i], getenv(tr[i]), &new_env[i]) == -1) {
                perror("Error asignando memoria para una variable de entorno");
                return;
            }
        }
        new_env[env_count] = NULL;
    }

    // Argumentos del programa
    char **prog_args = &tr[env_count];

    // Ejecutar el programa
    if (Execpve(prog_args, new_env, NULL) == -1) {
        perror("Imposible ejecutar");
        return;
    }

    // Liberar memoria del entorno si fue creada
    if (new_env != NULL) {
        free(new_env);
    }
}

void Cmd_ExecPri(char *tr[]) {
    if (tr[0] == NULL) {
        return; // No hacer nada si no hay argumentos
    }

    char **new_env = NULL; // Entorno reducido
    int env_count = 0;

    // Contar variables de entorno al inicio
    while (tr[env_count+1] != NULL && buscarEnEnviron(tr[env_count+1], NULL) != NULL) {
        env_count++;
    }

    // Crear el nuevo entorno si es necesario
    if (env_count > 0) {
        new_env = malloc((env_count + 1) * sizeof(char *));
        if (new_env == NULL) {
            perror("Error al asignar memoria para el entorno");
            return;
        }
        for (int i = 1; i < env_count+1; i++) {
            if (cambiarVariable(tr[i], getenv(tr[i]), &new_env[i-1]) == -1) {
                perror("Error asignando memoria para una variable de entorno");
                return;
            }
        }
        new_env[env_count] = NULL;
    }

    char *endptr;
    int priority = (int)strtol(tr[0], &endptr, 10);

    // Validar la prioridad
    if (*endptr != '\0' || errno == ERANGE) {
        errno = EACCES; // Simular error de permiso denegado
        perror("Imposible cambiar prioridad");
        perror("Imposible ejecutar");
        return;
    }

    if (tr[1] == NULL) {
        errno = EACCES; // Simular error de permiso denegado
        perror("Imposible cambiar prioridad");
        perror("Imposible ejecutar");
        return;
    }

    // Ejecutar el programa
    char **prog_args = &tr[env_count+1];
    if (Execpve(prog_args, new_env, &priority) == -1) {
        perror("Imposible ejecutar");
    }

    // Liberar memoria del entorno si fue creada
    if (new_env != NULL) {
        free(new_env);
    }
}

int Cmd_fg(char *tr[]) {
    if (tr[0] == NULL) {
        // Manejo de error si no se proporciona un programa
        errno = EFAULT;
        perror("No ejecutado");
        return -1;
    }

    char **new_env = NULL; // Entorno reducido
    int env_count = 0;

    // Contar variables de entorno al inicio
    while (tr[env_count] != NULL && buscarEnEnviron(tr[env_count], NULL) != NULL) {
        env_count++;
    }

    // Crear el nuevo entorno si es necesario
    if (env_count > 0) {
        new_env = malloc((env_count + 1) * sizeof(char *));
        if (new_env == NULL) {
            perror("Error al asignar memoria para el entorno");
            return -1;
        }
        for (int i = 0; i < env_count; i++) {
            if (cambiarVariable(tr[i], getenv(tr[i]), &new_env[i]) == -1) {
                perror("Error asignando memoria para una variable de entorno");
                return -1;
            }
        }
        new_env[env_count] = NULL;
    }

    // Argumentos del programa
    char **prog_args = &tr[env_count];

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fallo en el fork");
        return -1;
    }

    if (pid == 0) {
        if (env_count == 0) {
            // Proceso hijo: ejecutar el programa en primer plano
            if (Execpve(prog_args, environ, NULL) == -1) {
                perror("No ejecutado");
                return -2;
            }
        } else {
            if (Execpve(prog_args, new_env, NULL) == -1) {
                perror("No ejecutado");
                return -2;
            }
        }
    } else {
        // Proceso padre: esperar a que el proceso hijo termine
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("Error al esperar el proceso hijo");
        }
        // Liberar memoria del entorno si fue creada
        if (new_env != NULL) {
            free(new_env);
        }
    }
    return 0;
}

int Cmd_fgpri(char *tr[]) {
    if (tr[0] == NULL || tr[1] == NULL) {
        errno = EFAULT; // Dirección inválida
        perror("No ejecutado");
        return -1;
    }

    char **new_env = NULL; // Entorno reducido
    int env_count = 0;

    // Contar variables de entorno al inicio
    while (tr[env_count+1] != NULL && buscarEnEnviron(tr[env_count+1], NULL) != NULL) {
        env_count++;
    }

    // Crear el nuevo entorno si es necesario
    if (env_count > 0) {
        new_env = malloc((env_count + 1) * sizeof(char *));
        if (new_env == NULL) {
            perror("Error al asignar memoria para el entorno");
            return -1;
        }
        for (int i = 1; i < env_count+1; i++) {
            if (cambiarVariable(tr[i], getenv(tr[i]), &new_env[i-1]) == -1) {
                perror("Error asignando memoria para una variable de entorno");
                return -1;
            }
        }
        new_env[env_count] = NULL;
    }

    // Obtener la prioridad
    char *endptr;
    int prio = 1;
    prio = (int)strtol(tr[0], &endptr, 10);

    // Validar la prioridad
    if (*endptr != '\0' || errno == ERANGE) {
        errno = EACCES; // Simular error de permiso denegado
        perror("Imposible cambiar prioridad, establecida prioridad por defecto");
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fallo en el fork");
        return -1;
    }

    // Ejecutar el programa
    char **prog_args = &tr[env_count+1];
    if (pid == 0) {

        if (env_count == 0) {
            // Proceso hijo: ejecutar el programa en primer plano
            if (Execpve(prog_args, environ, &prio) == -1) {
                perror("No ejecutado");
                return -2;
            }
        } else {
            if (Execpve(prog_args, new_env, &prio) == -1) {
                perror("No ejecutado");
                return -2;
            }
        }
    } else {
        // Proceso padre: esperar al proceso hijo
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("Error al esperar el proceso hijo");
        }
        // Liberar memoria del entorno si fue creada
        if (new_env != NULL) {
            free(new_env);
        }
    }
    return 0;
}

void Cmd_Back(char *tr[])
{
    if (tr[0] == NULL) {
        errno = EFAULT; // Establecer un error genérico
        perror("Imposible ejecutar");
        return;
    }

    char **new_env = NULL; // Entorno reducido
    int env_count = 0;

    // Contar variables de entorno al inicio
        while (tr[env_count] != NULL && buscarEnEnviron(tr[env_count], NULL) != NULL) {
        env_count++;
    }

    // Crear el nuevo entorno si es necesario
    if (env_count > 0) {
        new_env = malloc((env_count + 1) * sizeof(char *));
        if (new_env == NULL) {
            perror("Error al asignar memoria para el entorno");
            return;
        }
        for (int i = 0; i < env_count; i++) {
            new_env[i] = getenv(tr[i]);
        }
        new_env[env_count] = NULL;
    }

    // Argumentos del programa
    char **prog_args = &tr[env_count];

    int pid = Execpve_back(prog_args, new_env, NULL);
    // Ejecutar el programa
    if (pid == -1) {
        perror("Imposible ejecutar");
        return;
    }

    char *full_name = strdup(prog_args[0]);
    for (int i = 1; prog_args[i] != NULL; i++) {
        size_t len = strlen(prog_args[i]) + strlen(full_name) + 2;
        full_name = realloc(full_name, len);
        if (full_name == NULL){
            perror("Error reasignando memoria para cadena de comando y argumentos");
            free(full_name);
            return;
        }
        strcat(full_name, " ");
        strcat(full_name, prog_args[i]);
    }

    anadirProceso(pid, full_name);

    // Liberar memoria del entorno si fue creada
    if (new_env != NULL) {
        free(new_env);
    }
}

void Cmd_BackPri(char *tr[]) {
    if (tr[0] == NULL) {
        return; // No hacer nada si no hay argumentos
    }

    char **new_env = NULL; // Entorno reducido
    int env_count = 0;

    // Contar variables de entorno al inicio
    while (tr[env_count+1] != NULL && buscarEnEnviron(tr[env_count+1], NULL) != NULL) {
        env_count++;
    }

    // Crear el nuevo entorno si es necesario
    if (env_count > 0) {
        new_env = malloc((env_count + 1) * sizeof(char *));
        if (new_env == NULL) {
            perror("Error al asignar memoria para el entorno");
            return;
        }
        for (int i = 1; i < env_count+1; i++) {
            if (cambiarVariable(tr[i], getenv(tr[i]), &new_env[i-1]) == -1) {
                perror("Error asignando memoria para una variable de entorno");
                return;
            }
        }
        new_env[env_count] = NULL;
    }

    char *endptr;
    int priority = (int)strtol(tr[0], &endptr, 10);

    // Validar la prioridad
    if (*endptr != '\0' || errno == ERANGE) {
        errno = EACCES; // Simular error de permiso denegado
        perror("Imposible cambiar prioridad");
        perror("Imposible ejecutar");
        return;
    }

    if (tr[1] == NULL) {
        errno = EACCES; // Simular error de permiso denegado
        perror("Imposible cambiar prioridad");
        perror("Imposible ejecutar");
        return;
    }

    // Ejecutar el programa
    char **prog_args = &tr[env_count+1];
    int pid = Execpve_back(prog_args, new_env, &priority);
    // Ejecutar el programa
    if (pid == -1) {
        perror("Imposible ejecutar");
        return;
    }

    char *full_name = strdup(prog_args[0]);
    for (int i = 1; prog_args[i] != NULL; i++) {
        size_t len = strlen(prog_args[i]) + strlen(full_name) + 2;
        full_name = realloc(full_name, len);
        if (full_name == NULL){
            perror("Error reasignando memoria para cadena de comando y argumentos");
            free(full_name);
            return;
        }
        strcat(full_name, " ");
        strcat(full_name, prog_args[i]);
    }

    anadirProceso(pid, full_name);

    // Liberar memoria del entorno si fue creada
    if (new_env != NULL) {
        free(new_env);
    }
}

void Cmd_Listjobs() {
    listarProcesos();
}

void Cmd_DelJobs(char *tr[]) {
    if (tr[0] == NULL) {
        listarProcesos();
        return;
    }
    int term_sig = -1;
    for (int i = 0; tr[i] != NULL; i++) {
        if (strcmp(tr[i], "-term") == 0) {
            term_sig = 0;
        } else if (strcmp(tr[i], "-sig") == 0) {
            term_sig = 1;
        }
    }

    if (term_sig == -1) {
        listarProcesos();
    } else {
        eliminarProcesos(term_sig);
        listarProcesos();
    }
}














