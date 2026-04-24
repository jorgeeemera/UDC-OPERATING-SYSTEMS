CFLAGS = -Wall -g

# Archivos fuente y cabecera
SRC = prompt.c cmd.c auxfunctions.c linkedlist.c processCommand.c p3.c
HEADERS = prompt.h cmd.h auxfunctions.h linkedlist.h processCommand.h includes.h memoryBlocks.h

# Nombre del ejecutable
TARGET = p3

# Regla por defecto, compilar el ejecutable
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(SRC) $(HEADERS)
	gcc $(CFLAGS) -o $(TARGET) $(SRC)

# Regla para limpiar los archivos generados
clean:
	rm -f $(TARGET)
