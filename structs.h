#ifndef STRUCTS_H
#define STRUCTS_H


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
} Heroe;

typedef struct {
	int id, vida,  dano, rango_ataque, rango_vision, estado;
	Coordenada posicion;
} Monstruo;


#endif // STRUCTS_H
