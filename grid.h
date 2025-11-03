#ifndef GRID_H
#define GRID_H

#include "structs.h"

Grid* crear_grid(int ancho, int alto);

void colocar_heroe(Grid *g, int x, int y);

void colocar_monstruo(Grid *g, int x, int y);

void print_grid(Grid *g);

void free_grid(Grid *g);

#endif //GRID_H
