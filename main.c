#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>    
#include "structs.h"
#include "grid.h"
#include "parser.h" 
#include "simulation.h" 

int main() {    
    Grid *map = NULL;
    Heroe heroe;
    Monstruo *monstruos = NULL;
    int num_monstruos = 0;

    parsear_config("config", &map, &heroe, &monstruos, &num_monstruos);
    
    if (map == NULL || monstruos == NULL) {
        printf("Error: El parser no pudo crear el mapa o los monstruos.\n");
        return 1;
    }

    if (pthread_mutex_init(&heroe.mutex, NULL) != 0) {
        perror("Error al inicializar mutex del héroe");
        return 1;
    }

    for (int i = 0; i < num_monstruos; i++) {
        if (pthread_mutex_init(&monstruos[i].mutex, NULL) != 0) {
            perror("Error al inicializar mutex del monstruo");
            return 1;
        }
        if (pthread_cond_init(&monstruos[i].cond_despertar, NULL) != 0) {
            perror("Error al inicializar cond del monstruo");
            return 1;
        }
    }

    printf("--- Mapa Inicial ---\n");
    colocar_heroe(map, heroe.posicion_actual.x, heroe.posicion_actual.y);
    for (int i = 0; i < num_monstruos; i++) {
        colocar_monstruo(map, monstruos[i].posicion.x, monstruos[i].posicion.y);
    }
    print_grid(map);

    iniciar_simulacion(map, &heroe, monstruos, num_monstruos);

    pthread_mutex_destroy(&heroe.mutex);
    for (int i = 0; i < num_monstruos; i++) {
        pthread_cond_destroy(&monstruos[i].cond_despertar);
        pthread_mutex_destroy(&monstruos[i].mutex);
    }

    free_grid(map);
    free(monstruos);
    if (heroe.ruta != NULL) {
        free(heroe.ruta);
    }
    return 0;
}
