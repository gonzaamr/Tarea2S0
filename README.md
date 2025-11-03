# Tarea 2 Sistemas operativos

## Descripción

Este proyecto implementa una simulación de batalla en C utilizando hilos. La simulación modela el comportamiento de un Héroe que sigue una ruta predefinida y un grupo de Monstruos que reaccionan a su presencia.  
Consta de varios módulos principales:

- **main.c:** Inicializa las estructuras (Héroe, Monstruos, Grid), los mutex y variables de condición, y lanza la simulación.
- **parser.c:** Lee un archivo de configuración (config) para establecer las estadísticas, posiciones y la ruta del héroe.
- **simulation.c:** Contiene la lógica central. Crea un hilo para el Héroe y un hilo para cada Monstruo, manejando sus acciones (moverse, atacar) y el estado del juego (victoria o derrota).
- **grid.c:** Funciones de utilidad para crear, modificar y liberar la memoria del mapa (no se usa para la lógica de movimiento, solo para la configuración inicial).

## Funcionamiento

El sistema se basa en hilos para manejar la lógica de cada entidad de forma concurrente:

* Hilo del Héroe: Sigue una ruta (definida en config). Si un monstruo entra en su rango de ataque, el héroe deja de moverse y ataca. Gana si llega al final de su ruta.
* Hilos de Monstruos: Cada monstruo tiene su propio hilo. Inician en estado "dormido". Se "despiertan" (estado de alerta) si ven al héroe o si otro monstruo cercano les alerta.
* Sincronización:
  - Mutex (pthread_mutex_t): Se usan para proteger los datos de cada entidad (vida, posición) y la variable global g_game_over, evitando condiciones de carrera cuando el héroe y un monstruo interactúan.
  - Variables de Condición (pthread_cond_t): Los monstruos "dormidos" esperan en un pthread_cond_timedwait. Son despertados por una señal (pthread_cond_signal) de otro monstruo que ha visto al héroe.
* El juego termina cuando el héroe llega a la meta (Victoria) o cuando su vida llega a 0 (Derrota).

## Uso e instalación 

### **Paso 1: Clonar el repositorio**  
```sh  
git clone https://github.com/gonzaamr/Tarea2S0
cd Tarea2S0 
```

### **Paso 2: Compila el codigo**  
Usa `gcc` y el archivo `Makefile` para compilar:  
```sh 
make
```

### **Paso 3: Ejecuta el archivo**  
```sh  
./doom 
```

##  Requerimientos  
- Linux   
- GCC Compiler (`sudo apt install gcc` para Ubuntu/WSL)
- Archivo de configuracion de parámetros del juego llamado `config` con el siguiente formato:
```sh  
# --- Configuración del Entorno ---
GRID_SIZE X X

# --- Configuración del Héroe ---
HERO_HP X
HERO_ATTACK_DAMAGE X
HERO_ATTACK_RANGE X
HERO_START X X
HERO_PATH (X,Y) (X,Y) (X,Y) (X,Y) (X,Y)

# --- Configuración de Monstruos ---
MONSTER_COUNT X

# Monstruo X
MONSTER_X_HP X
MONSTER_X_ATTACK_DAMAGE X
MONSTER_X_VISION_RANGE X
MONSTER_X_ATTACK_RANGE X
MONSTER_X_COORDS X X

```
