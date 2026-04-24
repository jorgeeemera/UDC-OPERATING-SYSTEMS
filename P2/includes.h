#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <grp.h>
#include <pwd.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/resource.h>

#define BUFFER_SIZE 1024 // Tamaño del buffer para leer datos
#define TAMANO 2048
#define MAXVAR 256
extern char **environ; // Variable global que contiene el entorno

#endif //INCLUDES_H