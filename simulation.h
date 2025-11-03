#ifndef SIMULATION_H
#define SIMULATION_H

#include "structs.h" // Para Grid, Heroe, Monstruo

/**
 * @brief Inicia la simulación completa.
 *
 * Esta función es el "cerebro" de todo. Se encarga de:
 * 1. Crear los hilos (1 para el héroe, M para los monstruos).
 * 2. Esperar a que todos los hilos terminen (pthread_join).
 * 3. Anunciar el resultado del juego.
 *
 * @param grid Puntero al mapa.
 * @param heroe Puntero al héroe.
 * @param monstruos Puntero al array de monstruos.
 * @param num_monstruos El número de monstruos.
 */
void iniciar_simulacion(Grid* grid, Heroe* heroe, Monstruo* monstruos, int num_monstruos);

#endif // SIMULATION_H
