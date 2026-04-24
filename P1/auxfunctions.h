#ifndef FILESOP_H
#define FILESOP_H

#include "linkedlist.h"
#include "includes.h"
#define BUFFER_FECHA_SIZE 20

struct file_info {
    int fd;
    char *name;
    int mode;
};

typedef struct file_info *file_d;

void terminarListaArchivos();

void anadirAFicherosAbiertos(int df, int mode, char *name);

void listarFicherosAbiertos();

void eliminarDeFicherosAbiertos(int df);

char* nombreFicheroDescriptor(int df);

char LetraTF (mode_t m);

char * ConvierteModo3 (mode_t m);

void mostrar_fecha_y_hora(char *buffer, size_t buffer_size, time_t tiempo);

void listDirectoryContents(char *dir_name, int long_format, int show_hidden, int acc_time, int show_link);

void listFileAux(char *nombre_archivo, struct stat file_info, int long_format, int acc_time, int is_link);

void listDirectoryContentsRecursively(char *dir_name, int long_format, int show_hidden, int acc_time, int show_link, bool after);

char* convertirModo(int mode);

bool deleteFileOrEmptyDirectory(char *path);

void deleteDirectoryContents(char *dir_name);

void deleteDirectoryContentsRecursively(char *dir_name);

void terminarListaComandos();

void anadirAComandos(char *command);

void listarComandos();

void listarNComandos(int n);

char* obtenerComando(int n);

#endif //FILESOP_H
