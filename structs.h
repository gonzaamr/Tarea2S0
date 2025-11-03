#ifndef STRUCTS_H
#define STRUCTS_H

#include <pthread.h>

typedef struct {
	int x;
	int y;
} Coordenada;


typedef struct {
	int ancho;
	int alto;
	char **celdas;
} Grid;

typedef struct {
	int vida, dano, rango_ataque;
	Coordenada *ruta;
	int largo_ruta;
	Coordenada posicion_actual;
	int indice_ruta_actual;
	pthread_mutex_t mutex;
} Heroe;

typedef struct {
	int id, vida,  dano, rango_ataque, rango_vision, estado;
	Coordenada posicion;
	pthread_mutex_t mutex;
	pthread_cond_t cond_despertar;
} Monstruo;


#endif // STRUCTS_H
