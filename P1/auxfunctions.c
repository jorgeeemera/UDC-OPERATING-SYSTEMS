#include "auxfunctions.h"

list lista_archivos; // Linked List que guarda información sobre los archivos abiertos

int numeroArchivos = 0; // Número de archivos abiertos

list lista_comandos; // Linked List que guarda los comandos utilizados

int num_comandos = 0; // Número de comandos que se han utilizado

// Añadir un archivo a la lista de archivos abiertos
void anadirAFicherosAbiertos(int df, int mode, char *name){
    size_t longitud_nombre = strlen(name);
    struct file_info *new_file = malloc(sizeof(struct file_info));  // Reservamos memoria para file_info
    new_file -> mode = mode;
    new_file -> name = (char*)malloc((longitud_nombre + 1)*sizeof(char));   // Reservamos memoria para el nombre
    strcpy(new_file -> name, name); // Copia el nombre a la memoria reservada para el nombre
    new_file -> fd = df;
    append(&lista_archivos, new_file);  // Añade el file_info recién creado a  la linked list
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
        printf("descriptor: %d -> %s %s\n", archivo->fd, archivo->name, flags);
        posicion = next(lista_archivos, posicion);
    }
}

// Terminar la lista de archivos cerrando cada archivo y liberando memoria
void terminarListaArchivos(){
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
    if (lista_comandos == NULL){
        printf("No hay comandos\n");
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

char* obtenerComando(int n){
    pos p = first(lista_comandos); // Obtener el primer nodo de la lista
    int i = 0;
    while (!is_end_of_list(lista_comandos, p) && i != n) {
        p = next(lista_comandos, p); // Pasar al siguiente nodo
        i ++;
    }
    return p -> data;
}
