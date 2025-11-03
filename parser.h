#ifndef PARSER_H
#define PARSER_H

#include "structs.h"
#include "grid.h"

void parsear_config(const char* filename, Grid **grid_ptr, Heroe *heroe_ptr, Monstruo **monstruos_ptr, int *num_monstruos);

#endif  // PARSER_H

