#include <stdio.h>
#include "grid.h"

int main(){

	Grid *map = crear_grid(10, 10);

	colocar_heroe(map, 2, 2),
	colocar_mounstro(map, 5, 5);
	colocar_mounstro(map, 8, 3);

	print_grid(map);
}
