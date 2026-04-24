#include "auxfunctions.h"
#include "memoryBlocks.h"

list lista_archivos; // Linked List que guarda información sobre los archivos abiertos
int numeroArchivos = 0; // Número de archivos abiertos

list lista_comandos; // Linked List que guarda los comandos utilizados
int num_comandos = 0; // Número de comandos que se han utilizado

list lista_memoria; // Linked List que guarda información sobre los bloques de memoria
int num_bloques = 0; // Número de bloques de memoria que se han reservado

list lista_variables_reservadas;

list search_list = NULL; // Lista global para la ruta de búsqueda

list background_process_list = NULL;

// Variables globales para ejemplo
int var_extr1, var_extr2, var_extr3; // Variables externas
int var_extr_init1 = 10, var_extr_init2 = 20, var_extr_init3 = 30; // Variables externas inicializadas
static int var_static1, var_static2, var_static3; // Variables estáticas
static int var_static_init1 = 1, var_static_init2 = 2, var_static_init3 = 3; // Variables estáticas inicializadas

// Añadir un archivo a la lista de archivos abiertos
void anadirAFicherosAbiertos(int df, int mode, char *name){
    size_t longitud_nombre = strlen(name);
    struct file_info *new_file = malloc(sizeof(struct file_info));  // Reservamos memoria para file_info
    new_file -> mode = mode;
    new_file -> name = (char*)malloc((longitud_nombre + 1)*sizeof(char));   // Reservamos memoria para el nombre
    strcpy(new_file -> name, name); // Copia el nombre a la memoria reservada para el nombre
    new_file -> fd = df;
    append(&lista_archivos, new_file);  // Añade el file_info recién creado a la linked list
    numeroArchivos++;
}

char* convertirModo(int mode){
    static char resultado[128];  // Usamos un buffer estático para almacenar la cadena
    resultado[0] = '\0';         // Inicializamos la cadena como vacía

    if (mode & O_CREAT) strcat(resultado, "O_CREAT ");
    if (mode & O_EXCL) strcat(resultado, "O_EXCL ");
    if (mode & O_RDONLY) strcat(resultado, "O_RDONLY ");
    if (mode & O_WRONLY) strcat(resultado, "O_WRONLY ");
    if (mode & O_RDWR) strcat(resultado, "O_RDWR ");
    if (mode & O_APPEND) strcat(resultado, "O_APPEND ");
    if (mode & O_TRUNC) strcat(resultado, "O_TRUNC ");

    // Elimina el último espacio en blanco (si hay uno)
    size_t len = strlen(resultado);
    if (len > 0 && resultado[len - 1] == ' ') {
        resultado[len - 1] = '\0';
    }

    return resultado;
}

// Recorre la lista enlazada desde el principio imprimiendo el nombre de cada archivo
void listarFicherosAbiertos() {
    printf("-Listado de archivos-\n");

    // Verificamos si la lista está vacía
    if (lista_archivos == NULL) {
        printf("No hay archivos abiertos\n");
        return;
    }

    pos posicion = lista_archivos;

    // Recorremos la lista imprimiendo la información de cada fichero
    while (posicion != NULL) {
        file_d archivo = posicion -> data;
        // Llamamos a la función para convertir el modo en flags legibles
        char* flags = convertirModo(archivo->mode);
        // Mostrar información del archivo
        printf("descriptor: %d -> %s %s\n", archivo->fd, archivo->name, flags);
        posicion = next(lista_archivos, posicion); // Avanzamos al siguiente nodo
    }
}

// Terminar la lista de archivos cerrando cada archivo y liberando memoria
void terminarListaArchivos() {
    if (lista_archivos == NULL) return;
    do{
        close(((file_d) first(lista_archivos) -> data) -> fd);  //Cerramos el archivo abierto
        free(((file_d) first(lista_archivos) -> data) -> name); //Liberamos la memoria que guarda el nombre del archivo
        free(first(lista_archivos) -> data);    //Liberamos el struct que contiene los datos del archivo
        delete_first_node(&lista_archivos); //Borramos el nodo
    } while (lista_archivos != NULL);
    numeroArchivos = 0;
}

// Eliminar un archivo de la lista de archivos abiertos, dado su descriptor
void eliminarDeFicherosAbiertos(int df){
    pos posicion = lista_archivos;
    pos posicionAnterior = NULL;
    // Recorremos la lista en busca del archivo con el descriptor
    while (posicion != NULL){
        file_d archivo = posicion -> data;
        if (archivo -> fd == df)
        {
            close(archivo -> fd);
            free(archivo -> name);
            free(archivo);
            numeroArchivos--;
            // Eliminamos el nodo correspondiente de la lista
            delete_node(&lista_archivos, posicionAnterior); /* Primera iteración: elimina el primer nodo
                                                             * Segunda iteración: elimina el segundo nodo
                                                             * El resto de iteraciones eliminan un nodo intermedio o el último*/
            return;
        }
        // Avanzamos al siguiente nodo
        posicionAnterior = posicion;
        posicion = next(lista_archivos, posicion);
    }
    // Si no se encuentra el descriptor, informamos al usuario
    printf("No se ha encontrado el archivo con descriptor %d\n", df);
}


char* nombreFicheroDescriptor(int df){
    pos p = first(lista_archivos); //Obtener el primer nodo de la lista
    // Recorremos la lista buscando el archivo con el descriptor dado
    while (!is_end_of_list(lista_archivos, p)) {
        struct file_info *info = get(lista_archivos, p);
        if (info -> fd == df) {
            return info-> name; //Retornar el nombre del archivo si se encuentra el descriptor
        }
        p = next(lista_archivos, p); // Avanzar al siguiente nodo
    }
    return "Descriptor no encontrado";
}

char LetraTF (mode_t m){
    switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /* fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
    }
}

char * ConvierteModo3 (mode_t m){
    char *permisos;

    if ((permisos=(char *) malloc (12))==NULL)
        return NULL;
    strcpy (permisos,"---------- ");

    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';

    return permisos;
}

void mostrar_fecha_y_hora(char *buffer, size_t buffer_size, time_t tiempo) {
    struct tm *time_info = localtime(&tiempo);
    strftime(buffer, buffer_size, "%Y/%m/%d-%H:%M", time_info);
}

void listDirectoryContents(char *dir_name, int long_format, int show_hidden, int acc_time, int show_link) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_info;
    char file_path[BUFFER_SIZE], fecha[20];

    // Imprimir el nombre del directorio con encabezado
    printf("************%s\n", dir_name);

    // Abrir el directorio y guardarlo en un struct dir
    if ((dir = opendir(dir_name)) == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    // Leer el contenido del directorio, iterando sobre todo su contenido
    while ((entry = readdir(dir)) != NULL) {
        // Si no se deben mostrar archivos ocultos, omitimos los que comienzan con '.'
        if (!show_hidden && entry->d_name[0] == '.') {
            continue;
        }

        // Construir la ruta completa al archivo
        snprintf(file_path, sizeof(file_path), "%s/%s", dir_name, entry->d_name);

        // Obtener información sobre el archivo o directorio
        if (lstat(file_path, &file_info) == -1) {
            perror("Error al obtener la información del archivo");
            continue;
        }

        // Mostrar el contenido en formato detallado (-long)
        if (long_format) {
            // Mostrar la fecha de modificación o de último acceso
            if (acc_time) {
                mostrar_fecha_y_hora(fecha, sizeof(char)*BUFFER_FECHA_SIZE, file_info.st_atime);
            } else {
                mostrar_fecha_y_hora(fecha, sizeof(char)*BUFFER_FECHA_SIZE, file_info.st_mtime);
            }

            // Obtener el nombre del propietario y del grupo
            struct passwd *propietario = getpwuid(file_info.st_uid);
            struct group *grupo = getgrgid(file_info.st_gid);

            // Convertir los permisos a un formato legible
            char *permisos = ConvierteModo3(file_info.st_mode);

            // Mostrar la información en el formato largo
            printf("%s  %2ld (%8ld)%s %s %s %8ld %s",
                   fecha,
                   (long)file_info.st_nlink,
                   (long)file_info.st_ino,
                   propietario->pw_name,
                   grupo->gr_name,
                   permisos,
                   file_info.st_size,
                   entry->d_name);

            // Si es un enlace simbólico y la opción -link está activada, mostrar el destino del enlace
            if (show_link && S_ISLNK(file_info.st_mode)) {
                char enlace[1024];
                ssize_t len = readlink(file_path, enlace, sizeof(enlace) - 1);
                if (len != -1) {
                    enlace[len] = '\0';  // Añadir terminador de cadena al final
                    printf(" -> %s", enlace);  // Mostrar el destino del enlace simbólico
                }
            }
            printf("\n");

            free(permisos);  // Liberar la memoria de la cadena de permisos
        } else if (acc_time) {
            // Mostrar formato de último acceso (-acc)
            mostrar_fecha_y_hora(fecha, sizeof(char)*BUFFER_FECHA_SIZE, file_info.st_atime);
            printf("%8ld  %s  %s\n", (long)file_info.st_size, fecha, entry->d_name);
        } else {
            // Si no es formato largo, mostrar solo el tamaño y el nombre
            if (show_link && S_ISLNK(file_info.st_mode)) {
                char enlace[1024];
                ssize_t len = readlink(file_path, enlace, sizeof(enlace) - 1);
                if (len != -1) {
                    enlace[len] = '\0';  // Terminar la cadena
                    printf("%8ld  %s -> %s\n", (long)file_info.st_size, entry->d_name, enlace);  // Mostrar el enlace
                }
            } else {
                printf("%8ld  %s\n", (long)file_info.st_size, entry->d_name);  // Mostrar archivos regulares
            }
        }
    }

    closedir(dir);
}

void listFileAux(char *nombre_archivo, struct stat file_info, int long_format, int acc_time, int is_link) {
    char fecha[BUFFER_FECHA_SIZE];
    // Si no se proporciona un archivo, devolver el directorio actual
    if (nombre_archivo == NULL) {
        char cwd[BUFFER_SIZE];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
            return;
        }
        perror("Error al obtener el directorio actual");
        return;
    }

    // Obtener información sobre el archivo
    // rellenando el struct file_info mediante la función lstat
    if (lstat(nombre_archivo, &file_info) == 0) {
        // Si no se especifican opciones, se muestra solo el tamaño y el nombre del archivo
        if (!long_format && !acc_time && !is_link) {
            printf("%8ld  %s\n", (long)file_info.st_size, nombre_archivo);
        }

        // Mostrar detalles en formato largo (-long)
        if (long_format) {
            // Convertir fecha y hora de modificación a formato YYYY/MM/DD-HH:MM
            mostrar_fecha_y_hora(fecha, sizeof(char)*BUFFER_FECHA_SIZE, file_info.st_mtime);

            // Obtener el nombre del propietario
            struct passwd *propietario = getpwuid(file_info.st_uid);
            struct group *grupo = getgrgid(file_info.st_gid);

            // Convertir los permisos a un formato legible
            char *permisos = ConvierteModo3(file_info.st_mode);

            // Mostrar la información en el formato solicitado
            printf("%s   1 (%ld) %s %s %s %8ld %s",
                   fecha,
                   (long)file_info.st_ino,
                   propietario->pw_name,
                   grupo->gr_name,
                   permisos,
                   file_info.st_size,
                   nombre_archivo);

            // Si es un enlace simbólico, mostrar el destino del enlace
            if (S_ISLNK(file_info.st_mode)) {
                char enlace[1024];
                ssize_t len = readlink(nombre_archivo, enlace, sizeof(enlace) - 1);
                if (len != -1) {
                    enlace[len] = '\0';  // Añadir terminador de cadena al final
                    printf(" -> %s", enlace);  // Mostrar el destino del enlace simbólico
                }
            }
            printf("\n");

            free(permisos);  // Liberar la memoria de la cadena de permisos
        }

        // Mostrar el tiempo de último acceso (-acc)
        if (acc_time) {
            // Convertir fecha y hora de último acceso a formato YYYY/MM/DD-HH:MM
            mostrar_fecha_y_hora(fecha, sizeof(char)*BUFFER_FECHA_SIZE, file_info.st_atime);

            // Mostrar el tamaño, fecha de acceso y el nombre del archivo
            printf("%8ld  %s  %s\n", (long)file_info.st_size, fecha, nombre_archivo);
        }

        // Mostrar el contenido del enlace simbólico (-link)
        if (is_link && S_ISLNK(file_info.st_mode)) {
            char enlace[1024];
            ssize_t len = readlink(nombre_archivo, enlace, sizeof(enlace) - 1);
            if (len != -1) {
                enlace[len] = '\0';  // Añadir terminador de cadena al final
                printf("%8ld  %s -> %s\n", (long)file_info.st_size, nombre_archivo, enlace);
            } else {
                perror("Error leyendo el enlace simbólico");
            }
        }

    } else {
        // En caso de error, imprimir que el archivo no existe
        printf("El archivo o carpeta %s no existe\n", nombre_archivo);
    }
}

void listDirectoryContentsRecursively(char *dir_name, int long_format, int show_hidden, int acc_time, int show_link, bool after) {
    char path[1024];
    path[0] = '\0';
    // Al principio de la ejecución, path solo contiene la ruta del directorio
    strcat(path, dir_name);
    // Si after es true, se muestran los subdirectorios después
    if (after) {
        listDirectoryContents(dir_name, long_format, show_hidden, acc_time, show_link);
    }

    DIR *dir;
    struct dirent *entry;

    // Abrir el directorio y guardarlo en un struct dir
    if ((dir = opendir(dir_name)) == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    // Leer el contenido del directorio, iterando sobre todo su contenido
    while ((entry = readdir(dir)) != NULL) {
        if (!show_hidden && entry->d_name[0] == '.') {
            continue;
        }

        DIR *dir2;
        // En aux guardamos el path a cada entrada del directorio
        char * aux = malloc(sizeof(char) * 1024);
        // Copiamos el path actual
        strcpy(aux, path);
        // Añadimos una barra
        strcat(aux, "/");
        // Añadimos el nombre de la entrada
        strcat(aux, entry->d_name);

        // Se excluyen el directorio actual, y el directorio anterior (. y ..) y se intenta abrir la entrada como un directorio
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && (dir2 = opendir(aux)) != NULL) {
            // Si la entrada se abre con éxito, llamar recursivamente a esta misma función
            listDirectoryContentsRecursively(aux, long_format, show_hidden, acc_time, show_link, after);
            if (dir2 != NULL) {
                closedir(dir2);
            }
        }
        free(aux);
    }

    // Si after es false, se muestran los subdirectorios antes
    if (!after){
        listDirectoryContents(dir_name, long_format, show_hidden, acc_time, show_link);
    }
    closedir(dir);
}

bool deleteFileOrEmptyDirectory(char *path) {
    struct stat file_info;
    // Obtener información sobre el archivo/directorio
    if (stat(path, &file_info) == -1) {
        perror("Error al obtener información del archivo/directorio");
        return false;
    }

    // Si es un fichero regular
    if (S_ISREG(file_info.st_mode)) {
        // Intentar borrar el archivo
        if (unlink(path) == -1) {
            perror("Error al eliminar el archivo");
        }
    }
    // Si es un directorio
    else if (S_ISDIR(file_info.st_mode)) {
        // Intentar borrar el directorio (solo si está vacío)
        if (rmdir(path) == -1) {
            // Si el error es que el directorio no está vacío
            if (errno == ENOTEMPTY) {
                printf("Imposible borrar %s: Directory not empty\n", path);
            } else {
                perror("Error al eliminar el directorio");
            }
        }
    } else {
        printf("'%s' no es ni un archivo ni un directorio válido\n", path);
    }
    //printf("archivo o directorio borrado: %s\n", path);
    return true;
}

void deleteDirectoryContents(char *dir_name) {
    DIR *dir;
    struct dirent *entry;
    char file_path[BUFFER_SIZE];

    // Abrir el directorio y guardarlo en un struct dir
    if ((dir = opendir(dir_name)) == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    // Leer el contenido del directorio
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // Construir la ruta completa al archivo
            snprintf(file_path, sizeof(file_path), "%s/%s", dir_name, entry->d_name);
            deleteFileOrEmptyDirectory(file_path);
        }
    }

    closedir(dir);
}

void deleteDirectoryContentsRecursively(char *dir_name) {
    char path[1024];
    path[0] = '\0';
    strcat(path, dir_name);
    DIR *dir;
    struct dirent *entry;

    // Abrir el directorio y guardarlo en un struct dir
    if ((dir = opendir(dir_name)) == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    // Leer el contenido del directorio, iterando sobre todo su contenido
    while ((entry = readdir(dir)) != NULL) {

        DIR *dir2;
        char * aux = malloc(sizeof(char) * 1024);
        strcpy(aux, path);
        strcat(aux, "/");
        strcat(aux, entry->d_name);

        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && (dir2 = opendir(aux)) != NULL) {
            deleteDirectoryContentsRecursively(aux);
            if (dir2 != NULL) {
                closedir(dir2);
            }
        }else if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            deleteFileOrEmptyDirectory(aux);
        }
        free(aux);
    }

    deleteDirectoryContents(dir_name);
    closedir(dir);
}

void terminarListaComandos(){
    if (lista_comandos == NULL) return;
    do{
        free(first(lista_comandos) -> data);    //Libera la memoria reservada para el comando
        delete_first_node(&lista_comandos);     //Elimina el nodo
    } while (lista_comandos != NULL);
    num_comandos = 0;
}

void anadirAComandos(char *command){
    size_t longitud_comando = strlen(command); // Longitud de la entrada
    char *new_command = malloc((longitud_comando + 1)*sizeof(char)); //Reservamos memoria parala longitud de entrada + el caracter \0
    strcpy(new_command, command);
    append(&lista_comandos, new_command); //Añade el nodo
    num_comandos++;
}

void listarComandos(){
    if (lista_comandos == NULL){
        printf("No hay comandos\n");
        return;
    }

    pos posicion = lista_comandos;
    int i = 0;

    //Recorre la lista imprimiendo cada comando
    while (posicion != NULL){
        char *command = posicion -> data;
        printf("%d -> %s", i, command);
        posicion = next(lista_comandos, posicion);
        i++;
    }
}

//Esta función solo imprime los últimos N comandos
void listarNComandos(int n){
    if (lista_comandos == NULL || num_comandos == 0){
        printf("No hay comandos en el historial\n");
        return;
    }

    if (n > num_comandos) {
        printf("Error: Se solicitaron %d comandos, pero solo hay %d en el historial.\n", n, num_comandos);
        return;
    }

    int indice = num_comandos - 1 - n;
    pos posicion = lista_comandos;
    int i = 0;

    while (posicion != NULL){
        char *command = posicion -> data;
        if(i > indice) printf("%d -> %s", i, command);
        posicion = next(lista_comandos, posicion);
        i++;
    }
}

// Devuelve el número de comandos en el historial
int obtenerNumeroComandos() {
    return num_comandos;
}

char* obtenerComando(int n){
    pos p = first(lista_comandos); // Obtener el primer nodo de la lista
    int i = 0;
    while (!is_end_of_list(lista_comandos, p) && i != n) {
        p = next(lista_comandos, p); // Pasar al siguiente nodo
        i ++;
    }
    return p -> data;
}

/**********************************************************************************************************************/
void Recursiva (int n)
{
    char automatico[TAMANO];
    static char estatico[TAMANO];
    // Imprime el valor del parámetro, su dirección, y las direcciones de los arrays
    printf ("parametro:%3d(%p) array %p, arr estatico %p\n",n,(void*)&n, (void*)automatico, (void*)estatico);
    // Llamada recursiva decreciendo el parámetro
    if (n>0)
    Recursiva(n-1);
}

void LlenarMemoria (void *p, size_t cont, unsigned char byte)
{
    // Convertir el puntero a tipo `unsigned char*` para trabajar byte a byte
    unsigned char *arr=(unsigned char *) p;
    size_t i;
    // Rellena `cont` bytes en la dirección `p` con el valor `byte
    for (i=0; i<cont;i++)
		arr[i]=byte;
}

void * ObtenerMemoriaShmget (key_t clave, size_t tam)
{
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;

    if (tam)     /*tam distito de 0 indica crear */
        flags=flags | IPC_CREAT | IPC_EXCL; /*cuando no es crear pasamos de tamano 0*/
    if (clave==IPC_PRIVATE)  /*no nos vale*/
        {errno=EINVAL; return NULL;}
    // shmat sirve para crear un segmento de memoria compartida
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    // shmat sirve para asociar a la memoria compartida el proceso
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
            // shmctl con IPC_RMID marca el segmento para la eliminación,
            // la memoria se libera cuando no está adjuntada a ningún proceso
             shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    // Actualizamos tamaño si estamos adjuntando un bloque existente
    if (!tam) shmctl(id, IPC_STAT, &s), tam = s.shm_segsz; /* si no es crear, necesitamos el tamano, que es s.shm_segsz*/
 /* Guardar en la lista   InsertarNodoShared (&L, p, s.shm_segsz, clave); */
    add_shared_block(p, tam, clave, id);
    return p;
}

void * MapearFichero (char * fichero, int protection)
{
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;
    // Configurar permisos de acceso
    if (protection&PROT_WRITE)
          modo=O_RDWR;
    // Validar archivo y abrirlo
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
          return NULL;
    // Mapear el archivo a la memoria
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
           return NULL;
    /* Guardar en la lista de descriptores usados df, fichero*/
    add_mmap_block(p, s.st_size, df, fichero);
    // Añadir el descriptor del archivo a la lista de abiertos
    char nombre_mmap[2048] = "";
    strcat(nombre_mmap,"Mapeo de ");
    strcat(nombre_mmap,fichero);
    anadirAFicherosAbiertos(df, modo, nombre_mmap);
    return p;
}

ssize_t LeerFichero (char *f, void *p, size_t cont)
{
    struct stat s;
    ssize_t  n;
    int df, aux;

    // Validar archivo y abrirlo
    if (stat (f,&s)==-1 || (df=open(f,O_RDONLY))==-1)
        return -1;
    if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont=s.st_size;
    // Leer datos del archivo
    if ((n=read(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    // Cerrar el archivo
    close (df);
    return n;
}

ssize_t EscribirFichero(char *f, void *p, size_t cont) {
    ssize_t n;
    int df, aux;

    // Abrir el archivo para escritura, creando uno nuevo si es necesario
    if ((df = open(f, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
        return -1;
    // Escribir datos desde el bloque de memoria al archivo
    if ((n = write(df, p, cont)) == -1) {
        aux = errno;
        close(df);
        errno = aux;
        return -1;
    }
    // Cerrar el archivo
    close(df);
    return n;
}

void Do_pmap (void) /*sin argumentos*/
 {
    pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
    char elpid[32];
    char *argv[4]={"pmap",elpid,NULL};
    // Obtener el PID del proceso actual
    sprintf (elpid,"%d", (int) getpid());
    if ((pid=fork())==-1){
        perror ("Imposible crear proceso");
        return;
    }
    if (pid==0){ // Proceso hijo
        if (execvp(argv[0],argv)==-1)
            perror("cannot execute pmap (linux, solaris)");

        argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;
        if (execvp(argv[0],argv)==-1)/*No hay pmap, probamos procstat FreeBSD */
            perror("cannot execute procstat (FreeBSD)");

        argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;
        if (execvp(argv[0],argv)==-1)  /*probamos procmap OpenBSD*/
            perror("cannot execute procmap (OpenBSD)");

        argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
        if (execvp(argv[0],argv)==-1) /*probamos vmmap Mac-OS*/
            perror("cannot execute vmmap (Mac-OS)");
        exit(1); // Finalizar si todos los comandos fallan
  }
    // Esperar a que el proceso hijo termine
    waitpid (pid,NULL,0);
}

// Añadir un bloque de memoria a la lista de bloques de memoria
void anadirBloqueMemoria(void* bloque){
    append(&lista_memoria, bloque);  // Añade el bloque recién creado a la linked list
    num_bloques++;  // Incrementa el contador global de bloques
}


void terminarListaMemoria() {
    if (lista_memoria == NULL) return;  // Si no hay bloques en la lista, salir
    do{
        // Dependiendo del tipo de bloque, realizar la liberación adecuada
        MemoryBlockType *blockType = (MemoryBlockType*)(first(lista_memoria)->data);
        if(*blockType == MALLOC) {
            malloc_block *block = (malloc_block*)(first(lista_memoria)->data);
            free(block->address);
        } else if(*blockType == SHARED) {
            shared_block *block = (shared_block*)(first(lista_memoria)->data);
            int shmid = block->shmid;
            if (shmid == -1) {
                perror("Error de shmget obteniendio el id de un bloque");
            }
            if (shmdt(block->address) == -1) {
                perror("Error de shmdt desvinculando memoria de un bloque");
            }
        } else if (*blockType == MMAP) {
            mmap_block *block = (mmap_block*)(first(lista_memoria)->data);
            if (munmap(block->address, block->size) == -1) {
                perror("Error al liberar memoria mapeada");
            }
            eliminarDeFicherosAbiertos(block->df);  // Elimina el descriptor del archivo asociado
            free(block->filename);  // Libera el nombre del archivo
        }
        free(first(lista_memoria)->data);   // Libera la memoria del bloque y elimina el nodo de la lista
        delete_first_node(&lista_memoria);  //Borramos el nodo
    } while (lista_memoria != NULL);
    num_bloques = 0;    // Restablecer el contador de bloques
}

void imprimirListaMemoria() {
    printf("******Lista de bloques asignados para el proceso %d\n", getpid());

    // Verificamos si la lista está vacía
    if (lista_memoria == NULL) {
        printf("No hay bloques de memoria reservados\n");
        return;
    }

    pos posicion = lista_memoria;

    // Recorremos la lista imprimiendo la información de cada bloque
    while (posicion != NULL) {
        MemoryBlockType type = *((MemoryBlockType*)posicion -> data);
        void* address = *((void**)posicion -> data + 1);
        size_t size = *((size_t*)posicion -> data + 2);
        time_t timestamp = *((time_t*)posicion -> data + 3);
        struct tm* tm = localtime(&timestamp);
        //printf("tipo: %d, address: %p , size: %ld, time: %s", type, address, size, asctime(tm));
        char time_buf[32];
        strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", tm);
        if (type == MALLOC) {
            printf("      %p               %ld %s malloc\n", address, size, time_buf);
        } else if (type == SHARED) {
            shared_block* block = (shared_block*)posicion->data;
            printf("      %p               %ld %s shared (key %d)\n", address, size, time_buf, block->key);
        } else if (type == MMAP) {
            mmap_block* block = (mmap_block*)posicion->data;
            printf("      %p               %ld %s %s  (descriptor %d)\n", address, size, time_buf, block->filename, block->df);
        }
        posicion = next(lista_memoria, posicion);
    }
}

const char* tipoMemoriaToString(MemoryBlockType type) {
    switch (type) {
    case MALLOC: return "malloc";
    case SHARED: return "shared";
    case MMAP: return "mmap";
    default: return "unknown";
    }
}

void imprimirlistaMemoriaPorTipo(MemoryBlockType type) {

    const char* tipo_str = tipoMemoriaToString(type); // Convierte el tipo a texto
    printf("******Lista de bloques asignados %s para el proceso %d\n", tipo_str, getpid());

    if (lista_memoria == NULL) {
        printf("No hay bloques de memoria reservados\n");
        return;
    }

    pos posicion = lista_memoria;
    while (posicion != NULL) {
        MemoryBlockType current_type = *((MemoryBlockType*)posicion->data);
        if (current_type == type) { // Imprime solo los bloques del tipo solicitado
            void* address = *((void**)posicion->data + 1);
            size_t size = *((size_t*)posicion->data + 2);
            time_t timestamp = *((time_t*)posicion->data + 3);
            struct tm* tm = localtime(&timestamp);
            char time_buf[32];
            strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", tm);

            if (type == MALLOC) {
                printf("      %p               %ld %s malloc\n", address, size, time_buf);
            } else if (type == SHARED) {
                shared_block* block = (shared_block*)posicion->data;
                printf("      %p               %ld %s shared (key %d)\n", address, size, time_buf, block->key);
            } else if (type == MMAP) {
                mmap_block* block = (mmap_block*)posicion->data;
                printf("      %p               %ld %s %s  (descriptor %d)\n", address, size, time_buf, block->filename, block->df);
            }
        }
        posicion = next(lista_memoria, posicion);
    }
}

void detachSharedBlock (const key_t clave) {
    pos posicion = lista_memoria;
    pos anterior = NULL;

    while (posicion != NULL) {
        MemoryBlockType type = *((MemoryBlockType*)posicion -> data);
        if (type == SHARED){    // Buscar bloques de memoria compartida
            shared_block *block = (shared_block *)posicion->data;
            if (block->key == clave) {  // Coincide la clave
                // Desvincular memoria compartida
                if (shmdt(block->address) == -1) {
                    perror("Error al desvincular memoria compartida");
                    return;
                } else {
                    printf("Memoria compartida de clave %lu desvinculada.\n", (unsigned long)clave);
                }
                // Eliminar de la lista
                delete_node(&lista_memoria, anterior);
                num_bloques--;
                free(block);
                return;
            }
        }
        anterior = posicion;
        posicion = next(lista_memoria, posicion);
    }
    printf("Error: No se encontró un bloque de memoria compartida con clave %lu\n", (unsigned long)clave);
}

void freeMallocBlockBySize (const int size) {
    pos posicion = lista_memoria;
    pos anterior = NULL;

    while (posicion != NULL) {
        MemoryBlockType type = *((MemoryBlockType*)posicion -> data);
        if (type == MALLOC){    // Verificar si el bloque es de tipo malloc
            malloc_block *block = (malloc_block *)posicion->data;
            if (block->size == size) {  // Coincide el tamaño del bloque
                free(block->address);   // Liberar bloque malloc
                delete_node(&lista_memoria, anterior);  // Eliminar de la lista
                num_bloques--;
                free(block);    // Liberar la estructura del bloque
                printf("Bloque malloc de %d bytes liberado\n", size);
                return;
            }
        }
        anterior = posicion;
        posicion = next(lista_memoria, posicion);   // Avanzar al siguiente bloque
    }
    printf("Error: No se encontró un bloque malloc con tamano %d\n", size);
}

void deallocateMmap(const char *filename) {
    pos posicion = lista_memoria;
    pos anterior = NULL;

    while (posicion != NULL) {
        MemoryBlockType type = *((MemoryBlockType*)posicion -> data);
        if (type == MMAP){  // Verificar si el bloque es de tipo mmap
            mmap_block *block = (mmap_block *)posicion->data;
            if (strcmp(block->filename, filename) == 0) {   // Coincide el nombre del archivo
                if (munmap(block->address, block->size) == -1) {    // Liberar bloque mmap
                    perror("Error al liberar memoria mapeada");
                    return;
                }
                eliminarDeFicherosAbiertos(block->df);  // Eliminar el descriptor del archivo de la lista
                free(block->filename);  // Liberar el nombre del archivo
                delete_node(&lista_memoria, anterior); // Eliminar bloque de la lista
                num_bloques--;
                free(block);
                printf("Archivo %s desmapeado y bloque liberado\n", filename);
                return;
            }
        }
        anterior = posicion;
        posicion = next(lista_memoria, posicion);   // Avanzar al siguiente bloque
    }
    printf("Error: No se encontró un bloque mmap con nombre de archivo %s\n", filename);
}

void deallocateAddress(void *addr) {
    pos posicion = lista_memoria;
    pos anterior = NULL;

    while (posicion != NULL) {
        const MemoryBlockType type = *((MemoryBlockType *)posicion->data);
        const void *address = *((void **)posicion->data + 1);

        if (address == addr) {  // Coincide la dirección
            malloc_block *block = NULL;
            shared_block *block2 = NULL;
            mmap_block *block3 = NULL;

            switch (type) {
                case MALLOC:
                    block = (malloc_block *)posicion->data;
                    free(block->address);   // Liberar bloque malloc
                    delete_node(&lista_memoria, anterior);  // Eliminar de la lista
                    num_bloques--;
                    free(block);
                    printf("Bloque malloc con direccion %p liberado\n", addr);
                    return;

                case SHARED:
                    block2 = (shared_block *)posicion->data;
                    if (shmdt(block2->address) == -1) { // Desvincular memoria compartida
                        perror("Error al desvincular memoria compartida");
                        return;
                    }
                    printf("Bloque shared con direccion %p desvinculado\n", addr);
                    delete_node(&lista_memoria, anterior);  // Eliminar de la lista
                    num_bloques--;
                    free(block2);
                    return;

                case MMAP:
                    block3 = (mmap_block *)posicion->data;
                    if (munmap(block3->address, block3->size) == -1) {  // Liberar bloque mmap
                        perror("Error al liberar memoria mapeada");
                        return;
                    }
                    eliminarDeFicherosAbiertos(block3->df); // Eliminar el descriptor del archivo de la lista
                    free(block3->filename);
                    delete_node(&lista_memoria, anterior);  // Eliminar de la lista
                    num_bloques--;
                    free(block3);
                    printf("Bloque mmap con direccion %p liberado\n", addr);
                    return;

                default:
                    perror("Bloque de memoria desconocido\n");
                    return;
            }
        }
        anterior = posicion;
        posicion = next(lista_memoria, posicion);   // Avanzar al siguiente bloque
    }
    // Mensaje si no se encuentra el bloque con la dirección dada
    printf("Direccion %p no asignada con malloc, shared o mmap\n", addr);
}

void imprimirVars() {
    int var_auto1, var_auto2, var_auto3; // Variables automáticas

    printf("Variables locales       %p,    %p,    %p\n",
           (void*)&var_auto1, (void*)&var_auto2, (void*)&var_auto3);

    printf("Variables globales      %p,    %p,    %p\n",
           (void*)&var_extr1, (void*)&var_extr2, (void*)&var_extr3);

    printf("Var (N.I.)globales      %p,    %p,    %p\n",
           (void*)&var_extr_init1, (void*)&var_extr_init2, (void*)&var_extr_init3);

    printf("Variables staticas      %p,    %p,    %p\n",
           (void*)&var_static1, (void*)&var_static2, (void*)&var_static3);

    printf("Var (N.I.)staticas      %p,    %p,    %p\n",
           (void*)&var_static_init1, (void*)&var_static_init2, (void*)&var_static_init3);
}

void imprimirFuncs() {
    printf("Funciones programa      %p,    %p,    %p\n",
           (void*)anadirAComandos, (void*)eliminarDeFicherosAbiertos, (void*)deallocateAddress);

    printf("Funciones libreria      %p,    %p,    %p\n",
           (void*)printf, (void*)malloc, (void*)free);
}

// Función auxiliar que lee un archivo y retorna su contenido
void* LeerContenido(const char *filename, size_t *size) {
    FILE *f = fopen(filename, "r");
    void *buffer;

    if (f == NULL) {
        return NULL;
    }

    // Obtener tamaño del archivo
    fseek(f, 0, SEEK_END);  // Moverse al final del archivo
    *size = ftell(f);   // Obtener la posición en el archivo (tamaño)
    fseek(f, 0, SEEK_SET);  // Volver al inicio del archivo

    // Asignar memoria
    buffer = malloc(*size);
    if (buffer == NULL) {
        fclose(f);  // Si no se puede asignar memoria, cerramos el archivo
        return NULL;
    }

    // Leer contenido
    if (fread(buffer, 1, *size, f) != *size) {
        free(buffer);   // Si no leemos todo el contenido, liberamos la memoria
        fclose(f);  // Y cerramos el archivo
        return NULL;
    }

    fclose(f);  // Cerramos el archivo después de leer
    return buffer;  // Retornamos el contenido leído
}

void *cadtop(const char *str) {
    if (str == NULL) {
        return NULL;
    }

    // Si no empieza con "0x" o "0X", asumimos que es una cadena normal
    if (str[0] != '0' || (str[1] != 'x' && str[1] != 'X')) {
        // Retornamos NULL para indicar que no es una dirección válida
        return NULL;
    }

    char *endptr;
    unsigned long addr = strtoul(str, &endptr, 16); // Convertir la cadena en un número hexadecimal

    if (*endptr != '\0') {
        return NULL;    // Si hay caracteres no numéricos después del número, es una dirección inválid
    }

    return (void *)addr;    // Retornamos la dirección como puntero
}

// Añadir bloque malloc
void* add_malloc_block(size_t size) {
    malloc_block *new_block = malloc(sizeof(malloc_block)); // Crear un nuevo bloque de memoria
    new_block->address = malloc(sizeof(char)*size); // Asignar memoria para el bloque
    unsigned char *bytes = (unsigned char*) new_block->address;
    for (size_t i = 0; i < size; i++){
        bytes[i] = rand() % 256;    // Rellenar el bloque con valores aleatorios
    }
    new_block->type = MALLOC;   // Especificar que el tipo de bloque es MALLOC
    new_block->size = size; // Almacenar el tamaño del bloque
    new_block->timestamp = time(NULL);  // Registrar el tiempo de asignación
    anadirBloqueMemoria(new_block); // Añadir el bloque a la lista de bloques
    return new_block->address;  // Retornar la dirección del bloque asignado
}

// Añadir bloque shared
void add_shared_block(void *p, size_t size, key_t key, int shmid) {
    shared_block *new_block = malloc(sizeof(shared_block)); // Crear un nuevo bloque de memoria compartida
    new_block->type = SHARED;   // Especificar que el tipo de bloque es SHARED
    new_block->address = p; // Almacenar la dirección del bloque
    new_block->size = size; // Almacenar el tamaño del bloque
    new_block->timestamp = time(NULL);  // Registrar el tiempo de asignación
    new_block->key = key;   // Almacenar la clave de memoria compartida
    new_block->shmid = shmid;   // Almacenar el ID del segmento de memoria compartida
    anadirBloqueMemoria(new_block); // Añadir el bloque a la lista de bloques de memoria
    printf("Un nuevo bloque se añadió a la lista\n");
}

// Añadir bloque mmap
void add_mmap_block(void *address, size_t size, int df, char *filename) {
    mmap_block *new_block = malloc(sizeof(mmap_block)); // Crear un nuevo bloque de memoria mapeada
    new_block->type = MMAP; // Especificar que el tipo de bloque es MMAP
    new_block->address = address;   // Almacenar la dirección del bloque
    new_block->size = size; // Almacenar el tamaño del bloque
    new_block->timestamp = time(NULL);  // Registrar el tiempo de asignación
    new_block->filename = strdup(filename); // Almacenar el nombre del archivo mapeado
    new_block->df = df; // Almacenar el descriptor de archivo
    anadirBloqueMemoria(new_block); // Añadir el bloque a la lista de bloques de memoria
    printf("Un nuevo bloque se añadió a la lista\n");
}

/**********************************************************************************************************************/

void getUidAux() {
    uid_t real_uid = getuid();       // Obtener UID real
    uid_t effective_uid = geteuid(); // Obtener UID efectivo

    struct passwd *real_pwd = getpwuid(real_uid);       // Obtener información de usuario real


    // Imprimir la credencial real
    if (real_pwd != NULL) {
        printf("Credencial real: %d, (%s)\n", real_uid, real_pwd->pw_name);
    } else {
        printf("Credencial real: %d, (desconocido)\n", real_uid);
    }

    struct passwd *effective_pwd = getpwuid(effective_uid); // Obtener información de usuario efectivo

    // Imprimir la credencial efectiva
    if (effective_pwd != NULL) {
        printf("Credencial efectiva: %d, (%s)\n", effective_uid, effective_pwd->pw_name);
    } else {
        printf("Credencial efectiva: %d, (desconocido)\n", effective_uid);
    }
}

// Función auxiliar para buscar en envp
char *buscarEnEnvp(char *key, char *envp[], char ***env_addr) {
    size_t key_len = strlen(key);
    for (char **env = envp; *env != NULL; env++) {
        if (strncmp(*env, key, key_len) == 0 && (*env)[key_len] == '=') {
            *env_addr = env; // Guardar la dirección de la variable en envp
            return *env + key_len + 1;
        }
    }
    return NULL;
}

// Función auxiliar para buscar en environ
char *buscarEnEnviron(char *key, char ***env_addr) {
    size_t key_len = strlen(key);
    for (char **env = environ; *env != NULL; env++) {
        if (strncmp(*env, key, key_len) == 0 && (*env)[key_len] == '=') {
            if (env_addr != NULL){
                *env_addr = env; // Guardar la dirección de la variable en environ
            }
            return *env + key_len + 1;
        }
    }
    return NULL;
}

// Función para mostrar todas las variables de entorno
void mostrarVars(char *envp[]) {
    for (int i = 0; envp[i] != NULL; i++) {
        char *value = strchr(envp[i], '=');
        if (value != NULL) {
            *value = '\0'; // Separar temporalmente nombre y valor
            printf("%p->main arg3[%d]=(%p) %s=%s\n", &envp[i], i, value + 1, envp[i], value + 1);
            *value = '='; // Restaurar la cadena original
        } else {
            printf("%p->main arg3[%d]=%s\n", &envp[i], i, envp[i]);
        }
    }
}

// Función para imprimir los resultados
void imprimirShowvar(char *key, char *value_envp, char **envp_addr,
                        char *value_environ, char **environ_addr, char *value_getenv) {
    // Imprimir resultado de envp
    if (value_envp != NULL) {
        printf("  Con arg3 main %s=%s(%p) @%p\n", key, value_envp, value_envp, envp_addr);
    } else {
        printf("  Con arg3 main %s no encontrada\n", key);
    }

    // Imprimir resultado de environ
    if (value_environ != NULL) {
        printf("  Con environ %s=%s(%p) @%p\n", key, value_environ, value_environ, environ_addr);
    } else {
        printf("  Con environ %s no encontrada\n", key);
    }

    // Imprimir resultado de getenv
    if (value_getenv != NULL) {
        printf("   Con getenv %s(%p)\n", value_getenv, value_getenv);
    } else {
        printf("   Con getenv %s no encontrada\n", key);
    }
}

// Función auxiliar para cambiar una variable en envp o environ
int cambiarVariable(char *var, char *val, char **env_addr) {
    size_t new_len = strlen(var) + strlen(val) + 2;
    char *new_entry = malloc(new_len);
    append(&lista_variables_reservadas, new_entry);
    if (new_entry == NULL) {
        perror("Imposible asignar memoria");
        return -1;
    }
    snprintf(new_entry, new_len, "%s=%s", var, val);
    *env_addr = new_entry; // Actualizar el puntero en envp o environ
    return 0;
}

// Función auxiliar para manejar errores
void manejarErrorCambio(const char *var) {
    errno = ENOENT;
    perror("Imposible cambiar variable");
}

void terminarListaVariables() {
    if (lista_variables_reservadas == NULL) return;
    do{
        free(first(lista_variables_reservadas)->data);
        delete_first_node(&lista_variables_reservadas); //Borramos el nodo
    } while (lista_variables_reservadas != NULL);
}

// Muestra las variables de entorno
void mostrarVarsConEnviron(char *env[], const char *tipo) {
    for (int i = 0; env[i] != NULL; i++) {
        char *value = strchr(env[i], '=');
        if (value != NULL) {
            *value = '\0'; // Temporalmente separar nombre y valor
            printf("%p->%s[%d]=(%p) %s=%s\n", &env[i], tipo, i, value + 1, env[i], value + 1);
            *value = '='; // Restaurar la cadena original
        } else {
            printf("%p->%s[%d]=%s\n", &env[i], tipo, i, env[i]);
        }
    }
}

// Muestra las direcciones de environ y envp
void mostrarDireccionesEnviron(char *envp[]) {
    printf("environ:   %p (almacenado en %p)\n", (void *)environ, (void *)&environ);
    printf("main arg3: %p (almacenado en %p)\n", (void *)envp, (void *)&envp);
}


char *SearchListFirst() {
    if (search_list == NULL) return NULL;
    return (char *)get(search_list, first(search_list));
}

char *SearchListNext(pos *current) {

    if (*current == NULL) {
        *current = first(search_list);
    } else {
        *current = next(search_list, *current);
    }

    if (is_end_of_list(search_list, *current)) {
        return NULL;
    }

    return (char *)get(search_list, *current);
}

bool SearchListContains(const char *dir) {
    pos current = first(search_list);
    while (current != NULL) {
        if (strcmp((char *)get(search_list, current), dir) == 0) {
            return true;
        }
        current = next(search_list, current);
    }
    return false;
}

void SearchListAdd(const char *dir) {
    if (SearchListContains(dir)) {
        return; // No imprimir mensaje si el directorio ya está
    }

    char *new_dir = strdup(dir);
    if (new_dir == NULL) {
        perror("Error al asignar memoria para el directorio");
        return;
    }

    append(&search_list, new_dir); // Agregar el directorio a la lista
}

void SearchListRemove(const char *dir) {
    pos prev = NULL, curr = first(search_list);

    while (curr != NULL) {
        if (strcmp((char *)get(search_list, curr), dir) == 0) {
            char *removed_dir = (char *)get(search_list, curr);
            if (prev == NULL) {
                delete_first_node(&search_list);
            } else {
                delete_next(search_list, prev);
            }
            free(removed_dir); // Liberar la memoria del directorio eliminado
            printf("Directorio %s eliminado de la lista de búsqueda\n", dir);
            return;
        }
        prev = curr;
        curr = next(search_list, curr);
    }
    printf("Directorio %s no encontrado en la lista de búsqueda\n", dir);
}


void SearchListClear() {
    while (first(search_list) != NULL) {
        char *dir = (char *)get(search_list, first(search_list));
        delete_first_node(&search_list);
        free(dir); // Liberar memoria de cada directorio
    }
    printf("Lista de búsqueda vaciada\n");
}


void SearchListImportFromPath() {
    char *path = getenv("PATH");
    if (path == NULL) {
        printf("No se pudo obtener el PATH del sistema\n");
        return;
    }

    char *path_copy = strdup(path); // Hacer una copia de PATH para tokenizar
    if (path_copy == NULL) {
        perror("Error al copiar el PATH");
        return;
    }

    char *token = strtok(path_copy, ":");
    int count = 0; // Contador de directorios añadidos

    while (token != NULL) {
        if (!SearchListContains(token)) { // Evitar duplicados
            SearchListAdd(token); // Agregar a la lista
            count++;
        }
        token = strtok(NULL, ":");
    }

    free(path_copy); // Liberar la copia de PATH
    printf("Importados %d directorios en la ruta de búsqueda\n", count);
}

void SearchListPrint() {
    pos current = first(search_list);
    if (current == NULL) {
        printf("La lista de búsqueda está vacía\n");
        return;
    }
    while (current != NULL) {
        printf("%s\n", (char *)get(search_list, current));
        current = next(search_list, current);
    }
}

void terminarListaSearch() {
    if (search_list == NULL) return;
    do{
        free(first(search_list)->data);
        delete_first_node(&search_list); //Borramos el nodo
    } while (search_list != NULL);
}

char *Ejecutable(char *s) {
    static char path[PATH_MAX];
    struct stat st;
    char *p;

    // Si no se especifica un nombre de archivo
    if (s == NULL || (p = SearchListFirst()) == NULL) {
        errno = ENOENT; // No existe el archivo o directorio
        return NULL;
    }

    // Si es una ruta absoluta o relativa
    if (s[0] == '/' || !strncmp(s, "./", 2) || !strncmp(s, "../", 3)) {
        if (lstat(s, &st) != -1) {
            return s; // Ruta válida
        }
        errno = ENOENT; // Ruta inválida
        return NULL;
    }

    pos current = NULL;
    // Buscar en la lista de búsqueda
    do {
        strncpy(path, p, PATH_MAX - 1);
        strncat(path, "/", PATH_MAX - 1);
        strncat(path, s, PATH_MAX - 1);
        if (lstat(path, &st) != -1) {
            return path; // Encontrado
        }
    } while ((p = SearchListNext(&current)) != NULL);

    errno = ENOENT; // No se encontró el ejecutable en la lista de búsqueda
    return NULL;
}

int Execpve(char *tr[], char **new_env, const int *pprio) {
    char *path;

    // Verificar que el ejecutable exista
    if (tr[0] == NULL || (path = Ejecutable(tr[0])) == NULL) {
        return -1; // `Ejecutable` ya ajusta `errno` según el problema
    }

    // Cambiar la prioridad si es necesario
    if (pprio != NULL && setpriority(PRIO_PROCESS, getpid(), *pprio) == -1) {
        perror("Imposible cambiar prioridad");
        return -1;
    }

    // Ejecutar el programa
    if (new_env == NULL) {
        return execv(path, tr);
    }
    return execve(path, tr, new_env);
}

int Execpve_back(char *tr[], char **new_env, const int *pprio) {
    char *path;

    // Verificar que el ejecutable exista
    if (tr[0] == NULL || (path = Ejecutable(tr[0])) == NULL) {
        perror("Error buscando el ejecutable");
        return -1; // `Ejecutable` ya ajusta `errno` según el problema
    }

    // Ejecutar el programa
    pid_t pid;
    if ((pid = fork()) == -1) {
        perror("Fallo en el fork");
        return -1;
    }

    // Crear un nuevo proceso usando fork
    if (pid == 0) {
        // Cambiar la prioridad si es necesario
        if (pprio != NULL && setpriority(PRIO_PROCESS, getpid(), *pprio) == -1) {
            perror("Imposible cambiar prioridad");
            return -1;
        }

        if (new_env == NULL) {
            return execv(path, tr);
        }
        return execve(path, tr, new_env); // Evita salir del proceso hijo
    }

    if (pid > 0) {
        return pid; // No esperar al proceso hijo
    }
    return -1;
}

void anadirProceso(pid_t pid, char *command_line) {
    process_info_d new_process = malloc(sizeof(struct process_info));
    if (new_process == NULL) {
        perror("Error al agregar proceso a la lista");
        return;
    }

    new_process->pid = pid;
    new_process->timestamp = time(NULL);
    new_process->status = ACTIVE; // ACTIVO
    new_process->command_line = command_line;
    new_process->return_signal = 0;

    append(&background_process_list, new_process);
}

void listarProcesos() {
    uid_t real_uid = getuid();
    struct passwd *real_pwd = getpwuid(real_uid);
    char *name = real_pwd->pw_name;

    // Verificamos si la lista está vacía
    if (background_process_list == NULL) {
        printf("No hay procesos en segundo plano\n");
        return;
    }

    pos posicion = background_process_list;

    // Recorremos la lista imprimiendo la información de cada proceso
    while (posicion != NULL) {
        process_info_d proceso = posicion -> data;
        int status;
        pid_t process_pid = waitpid(proceso->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        int priority = getpriority(PRIO_PROCESS, proceso->pid);
        struct tm* tm = localtime(&(proceso->timestamp));
        char time_buf[32];
        strftime(time_buf, sizeof(time_buf), "%d/%m/%Y %H:%M:%S", tm);

        if (process_pid != 0 && (proceso->status == ACTIVE || proceso ->status == STOPPED)){
            if (WIFEXITED(status)) {
                proceso->status = FINISHED;
                proceso->return_signal = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                proceso->status = SIGNALED;
                proceso->return_signal = WTERMSIG(status);
            } else if (WIFSTOPPED(status)) {
                proceso->status = STOPPED;
                proceso->return_signal = WEXITSTATUS(status);
            } else if (WIFCONTINUED(status)) {
                proceso->status = ACTIVE;
                proceso->return_signal = 0;
            }
        }
        if (proceso->status == SIGNALED)
        {
            // Mostrar información del proceso
            printf("%d  %s  p=%d  %s  %s  (%s)  %s\n", proceso->pid, name, priority, time_buf, estado_a_cadena(proceso->status),
                NombreSenal(proceso->return_signal), proceso->command_line);
        } else {
            printf("%d  %s  p=%d  %s  %s  (%03d)  %s\n", proceso->pid, name, priority, time_buf, estado_a_cadena(proceso->status),
                proceso->return_signal, proceso->command_line);
        }
        posicion = next(background_process_list, posicion); // Avanzamos al siguiente nodo
    }
}

const char *estado_a_cadena(enum status_types status) {
    switch (status)
    {
        case ACTIVE:
            return "ACTIVE";
        case FINISHED:
            return "FINISHED";
        case SIGNALED:
            return "SIGNALED";
        case STOPPED:
            return "STOPPED";
        default:
            return "UNKNOWN";
    }
}

void terminarListaProcesos() {
    if (background_process_list == NULL) return;
    do{
        free(((process_info_d) first(background_process_list) -> data) -> command_line); // Liberamos la memoria que guarda el nombre del comando
        free(first(background_process_list) -> data); // Liberamos el struct que contiene los datos del comando
        delete_first_node(&background_process_list); // Borramos el nodo
    } while (background_process_list != NULL);
}

/*las siguientes funciones nos permiten obtener el nombre de una senal a partir
del número y viceversa */
static struct SEN sigstrnum[]={
    {"HUP", SIGHUP},
    {"INT", SIGINT},
    {"QUIT", SIGQUIT},
    {"ILL", SIGILL},
    {"TRAP", SIGTRAP},
    {"ABRT", SIGABRT},
    {"IOT", SIGIOT},
    {"BUS", SIGBUS},
    {"FPE", SIGFPE},
    {"KILL", SIGKILL},
    {"USR1", SIGUSR1},
    {"SEGV", SIGSEGV},
    {"USR2", SIGUSR2},
    {"PIPE", SIGPIPE},
    {"ALRM", SIGALRM},
    {"TERM", SIGTERM},
    {"CHLD", SIGCHLD},
    {"CONT", SIGCONT},
    {"STOP", SIGSTOP},
    {"TSTP", SIGTSTP},
    {"TTIN", SIGTTIN},
    {"TTOU", SIGTTOU},
    {"URG", SIGURG},
    {"XCPU", SIGXCPU},
    {"XFSZ", SIGXFSZ},
    {"VTALRM", SIGVTALRM},
    {"PROF", SIGPROF},
    {"WINCH", SIGWINCH},
    {"IO", SIGIO},
    {"SYS", SIGSYS},
/*senales que no hay en todas partes*/
#ifdef SIGPOLL
    {"POLL", SIGPOLL},
#endif
#ifdef SIGPWR
    {"PWR", SIGPWR},
#endif
#ifdef SIGEMT
    {"EMT", SIGEMT},
#endif
#ifdef SIGINFO
    {"INFO", SIGINFO},
#endif
#ifdef SIGSTKFLT
    {"STKFLT", SIGSTKFLT},
#endif
#ifdef SIGCLD
    {"CLD", SIGCLD},
#endif
#ifdef SIGLOST
    {"LOST", SIGLOST},
#endif
#ifdef SIGCANCEL
    {"CANCEL", SIGCANCEL},
#endif
#ifdef SIGTHAW
    {"THAW", SIGTHAW},
#endif
#ifdef SIGFREEZE
    {"FREEZE", SIGFREEZE},
#endif
#ifdef SIGLWP
    {"LWP", SIGLWP},
#endif
#ifdef SIGWAITING
    {"WAITING", SIGWAITING},
#endif
     {NULL,-1},
    };    /*fin array sigstrnum */

int ValorSenal(char * sen)  /*devuelve el numero de senal a partir del nombre*/
{
    int i;
    for (i=0; sigstrnum[i].name!=NULL; i++)
        if (!strcmp(sen, sigstrnum[i].name))
            return sigstrnum[i].signal;
    return -1;
}


char *NombreSenal(int sen)  /*devuelve el nombre senal a partir de la senal*/
{			/* para sitios donde no hay sig2str*/
    int i;
    for (i=0; sigstrnum[i].name!=NULL; i++)
        if (sen==sigstrnum[i].signal)
            return sigstrnum[i].name;
    return ("SIGUNKNOWN");
}

void eliminarProcesos(int term_sig) {
    pos posicion = background_process_list;
    pos anterior = NULL;

    // Recorremos la lista eliminando los procesos que correspondan
    while (posicion != NULL) {
        process_info_d proceso = posicion -> data;
        if (proceso->status == FINISHED && term_sig == 0) {
            free(proceso->command_line);   // Liberar nombre de comando
            delete_node(&background_process_list, anterior);  // Eliminar de la lista
            free(proceso);    // Liberar la estructura del proceso
            posicion = next(background_process_list, anterior);
        } else if (proceso->status == SIGNALED && term_sig == 1) {
            free(proceso->command_line);   // Liberar nombre de comando
            delete_node(&background_process_list, anterior);  // Eliminar de la lista
            free(proceso);    // Liberar la estructura del proceso
            posicion = next(background_process_list, anterior);
        } else {
            anterior = posicion;
            posicion = next(background_process_list, posicion);
        }
    }
}




