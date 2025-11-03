# --- Variables de Compilación ---

# El compilador que usaremos
CC = gcc

# Banderas (flags) para el compilador:
# -Wall: Muestra TODAS las advertencias (Warnings).
# -g:    Incluye información de "debug" (para depurar).
# -pthread: ¡CRUCIAL! Activa la librería de hilos (pthreads).
CFLAGS = -Wall -g -pthread

# Bibliotecas (libraries) adicionales para el enlace:
# -lm:     Librería de Matemáticas (por abs() y clock_gettime() en simulation.c).
# -pthread: Lo ponemos aquí también para asegurar el enlace correcto.
LIBS = -lm -pthread

# --- Variables del Proyecto ---

# El nombre que tendrá tu programa ejecutable final
TARGET = doom

# La lista de TODOS los archivos "objeto" (.o) que necesita tu proyecto.
# Son tus 4 archivos .c convertidos a .o
OBJS = main.o grid.o parser.o simulation.o

# --- Reglas de Construcción ---

# La primera regla es la que se ejecuta por defecto (cuando solo escribes 'make')
# Le decimos que "todo" (all) depende de nuestro ejecutable $(TARGET)
all: $(TARGET)

# Regla de Enlace (Linking):
# Cómo construir el ejecutable $(TARGET) a partir de TODOS los .o
# $(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)
# se traduce en:
# gcc -Wall -g -pthread -o doom main.o grid.o parser.o simulation.o -lm -pthread
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# --- Reglas de Compilación ---
# Aquí le decimos cómo construir CADA archivo .o y de qué archivos .h depende.
# Si cambias un .h, Makefile sabrá qué .o debe volver a compilar.

# main.o depende de todos los "menús" (.h) que incluye
main.o: main.c structs.h grid.h parser.h simulation.h
	$(CC) $(CFLAGS) -c main.c

# grid.o depende de su menú y de los structs
grid.o: grid.c grid.h structs.h
	$(CC) $(CFLAGS) -c grid.c

# parser.o depende de su menú, structs y grid.h (porque llama a crear_grid)
parser.o: parser.c parser.h structs.h grid.h
	$(CC) $(CFLAGS) -c parser.c

# simulation.o depende de su menú, structs y grid.h (porque llama a print_grid)
simulation.o: simulation.c simulation.h structs.h grid.h
	$(CC) $(CFLAGS) -c simulation.c

# --- Regla de Limpieza ---

# Esto es para limpiar tu carpeta.
# Si escribes 'make clean', se borrará el ejecutable y todos los .o
clean:
	rm -f $(TARGET) $(OBJS)
