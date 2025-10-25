
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>

Grid* crear_grid(int ancho, int alto){
	Grid *g = malloc(sizeof(Grid));
	g->ancho = ancho;
	g->alto = alto;

	g->celdas = malloc(alto * sizeof(char*));
	for(int i = 0; i < alto ; i++){
		g->celdas[i] = malloc(ancho * sizeof(char));
		for (int j = 0; j < ancho; j++){
			g->celdas[i][j] = '.';
		}
	}
	return g;
}

void colocar_heroe(Grid *g, int x, int y) {
	if (x >= 0 && x < g->ancho && y >= 0 && y < g->alto) {
		g->celdas[y][x] = 'H';
	}
}

void colocar_mounstro(Grid *g, int x, int y) {
	if (x >= 0 && x < g->ancho && y >= 0 && y < g->alto) {
		g->celdas[y][x] = 'M';
	}
}



void print_grid(Grid *g){
	for(int y = 0; y < g->alto; y++){
		for(int x = 0; x < g ->ancho ; x++){
			printf("%c", g->celdas[y][x]);
		}
		printf("\n");
	}
}

