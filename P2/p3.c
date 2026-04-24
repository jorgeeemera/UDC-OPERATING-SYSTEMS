// Jorge Mera Hermida - jorge.mera@udc.es
// Adrián Novio Vilariño - adrian.nvilarino@udc.es

#include "includes.h"
#include "prompt.h"
#include "processCommand.h"
#include "auxfunctions.h"

list historial = NULL; // Almacena el historial de entradas ingresadas

int main(int argc, char *argv[], char *envp[]) { // Se incluye el tercer argumento envp[]

    anadirAFicherosAbiertos(0, O_RDWR, "entrada estandar");
    anadirAFicherosAbiertos(1, O_RDWR, "salida estandar");
    anadirAFicherosAbiertos(2, O_RDWR, "error estandar");

    char *buffer = malloc(sizeof(char) * BUFFERSIZE); // Se reserva memoria para almacenar el comando ingresado por el usuario
    char *trozos[100]; // Array de punteros (char*) que almacenará las palabras separadas de cada comando
    bool terminado = false; // Variable que se hace true si procesarEntrada finaliza la shell
    while (!terminado) {
        imprimirPrompt();
        leerEntrada(buffer); // Almacena la línea ingresada por el usuario en buffer
        anadirAComandos(buffer); // Añade el comando completo al historial de comandos almacenado en la lista enlazada

        // Se pasa envp como argumento a procesarEntrada
        terminado = procesarEntrada(buffer, trozos, envp);
        // Separa el comando y sus argumentos en trozos
        // Identifica el comando y ejecuta la función adecuada
    }
    free(buffer); // Libera la memoria reservada para el buffer que almacenaba la entrada del usuario
    terminarListaArchivos(); // Cierra y libera todos los archivos abiertos en la lista de archivos
    terminarListaComandos(); // Libera la memoria asociada a cada comando en el historial
    terminarListaMemoria();
    terminarListaVariables();
    terminarListaSearch();
    terminarListaProcesos();
}


