#ifndef AUXFUNCTIONS_H
#define AUXFUNCTIONS_H

#include "linkedlist.h"
#include "includes.h"
#include "memoryBlocks.h"
#define BUFFER_FECHA_SIZE 20
#define TAMANO 2048

struct file_info {
    int fd;
    char *name;
    int mode;
};

typedef struct file_info *file_d;

struct SEN {
    char *name;
    int signal;
};

enum status_types {
    FINISHED, STOPPED, SIGNALED, ACTIVE
};

struct process_info {
    pid_t pid;
    time_t timestamp;
    enum status_types status;
    int return_signal;
    char *command_line;
};

typedef struct process_info *process_info_d;

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

int obtenerNumeroComandos();

char* obtenerComando(int n);

void Recursiva (int n);

void LlenarMemoria (void *p, size_t cont, unsigned char byte);

void * ObtenerMemoriaShmget (key_t clave, size_t tam);

void * MapearFichero (char * fichero, int protection);

ssize_t LeerFichero (char *f, void *p, size_t cont);

ssize_t EscribirFichero(char *f, void *p, size_t cont);

void Cmd_ReadFile (char *tr[]);

void Do_pmap (void);

void* add_malloc_block(size_t size);

void add_shared_block(void *p, size_t size, key_t key, int shmid);

void add_mmap_block(void *address, size_t size, int df, char *filename);

void imprimirListaMemoria();

void terminarListaMemoria();

const char* tipoMemoriaToString(MemoryBlockType type);

void imprimirlistaMemoriaPorTipo(MemoryBlockType type);

void detachSharedBlock (key_t clave);

void freeMallocBlockBySize (const int size);

void deallocateMmap(const char *filename);

void deallocateAddress (void *addr);

void imprimirVars();

void imprimirFuncs();

void *cadtop(const char *str);

void getUidAux();

char *buscarEnEnvp(char *key, char *envp[], char ***env_addr);

char *buscarEnEnviron(char *key, char ***env_addr);

void mostrarVars(char *envp[]);

void imprimirShowvar(char *key, char *value_envp, char **envp_addr,
                     char *value_environ, char **environ_addr, char *value_getenv);

int cambiarVariable(char *var, char *val, char **env_addr);

void manejarErrorCambio(const char *var);

void terminarListaVariables();

void mostrarVarsConEnviron(char *env[], const char *tipo);

void mostrarDireccionesEnviron(char *envp[]);

char *SearchListFirst();

char *SearchListNext();

bool SearchListContains(const char *dir);

void SearchListAdd(const char *dir);

void SearchListRemove(const char *dir);

void SearchListClear();

void SearchListImportFromPath();

void SearchListPrint();

void terminarListaSearch();

char * Ejecutable (char *s);

int Execpve(char *tr[], char **NewEnv, const int * pprio);

int Execpve_back(char *tr[], char **new_env, const int *pprio);

void anadirProceso(pid_t pid, char *command_line);

void listarProcesos();

const char *estado_a_cadena(enum status_types status);

void terminarListaProcesos();

int ValorSenal(char * sen);

char *NombreSenal(int sen);

const char *estado_a_cadena(enum status_types status);

void listarProcesos();

void eliminarProcesos(int term_sig);


#endif //AUXFUNCTIONS_H
