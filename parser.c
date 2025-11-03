#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 256

static void parsear_hero_path(char* line, Heroe* heroe) {
    	int num_coordenadas = 0;
    	for (char* p = line; *p; p++) {
        	if (*p == '(') {
            		num_coordenadas++;
        	}
    	}

   	if (num_coordenadas == 0) {
        	heroe->largo_ruta = 0;
        	heroe->ruta = NULL;
        	return;
    	}

    	heroe->largo_ruta = num_coordenadas;
    	heroe->ruta = malloc(sizeof(Coordenada) * num_coordenadas);
    	if (heroe->ruta == NULL) exit(1);

    	char* cursor = line;
    	int i = 0;
    	cursor = strchr(cursor, '(');

    	while (cursor != NULL && i < num_coordenadas) {
        	if (sscanf(cursor, "(%d,%d)", &heroe->ruta[i].x, &heroe->ruta[i].y) == 2) {
			i++;
        	}
        	cursor = strchr(cursor + 1, '(');
    	}
    	heroe->largo_ruta = i;
}


void parsear_config(const char* filename, Grid **grid_ptr, Heroe *heroe_ptr, Monstruo **monstruos_ptr, int *num_monstruos) {
   	FILE *file = fopen(filename, "r");
    	if (file == NULL) {
        	perror("Error al abrir el archivo de configuración");
        	exit(1);
    	}

    	char line_buffer[MAX_LINE_LENGTH];
    	Monstruo *array_monstruos = NULL; 

    	while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
        	line_buffer[strcspn(line_buffer, "\n")] = 0;
        	char *line = line_buffer;
        	while (isspace((unsigned char)*line)) {
           	 	line++;
        	}

        	if (*line == '\0' || *line == '#') {
            		continue;
        	}

        	if (strstr(line, "GRID_SIZE") == line) {
            		int ancho, alto;
            		if (sscanf(line, "GRID_SIZE %d %d", &ancho, &alto) == 2) {
                		*grid_ptr = crear_grid(ancho, alto);
            		}
        	} else if (strstr(line, "HERO_HP") == line) {
            		sscanf(line, "HERO_HP %d", &heroe_ptr->vida);
        	} else if (strstr(line, "HERO_ATTACK_DAMAGE") == line) {
            		sscanf(line, "HERO_ATTACK_DAMAGE %d", &heroe_ptr->dano);
        	} else if (strstr(line, "HERO_ATTACK_RANGE") == line) {
            		sscanf(line, "HERO_ATTACK_RANGE %d", &heroe_ptr->rango_ataque);
        	} else if (strstr(line, "HERO_START") == line) {
            		sscanf(line, "HERO_START %d %d", &heroe_ptr->posicion_actual.x, &heroe_ptr->posicion_actual.y);
            		heroe_ptr->indice_ruta_actual = 0;
        	} else if (strstr(line, "HERO_PATH") == line) {
            		parsear_hero_path(line, heroe_ptr);
        	} else if (strstr(line, "MONSTER_COUNT") == line) {
            		sscanf(line, "MONSTER_COUNT %d", num_monstruos);
            		array_monstruos = malloc(sizeof(Monstruo) * (*num_monstruos));
            		if (array_monstruos == NULL) exit(1);
            		for(int i = 0; i < *num_monstruos; i++) {
                		array_monstruos[i].id = i + 1;
                		array_monstruos[i].vida = 0;
                		array_monstruos[i].posicion.x = 0;
                		array_monstruos[i].posicion.y = 0;
                		array_monstruos[i].estado = 0;
            		}
            		*monstruos_ptr = array_monstruos;
        	} else if (strstr(line, "MONSTER_") == line) {
            		int id;
            		char key[50];
            		if (sscanf(line, "MONSTER_%d_%s", &id, key) == 2) {
               			if (array_monstruos != NULL && id > 0 && id <= *num_monstruos) {
                    			Monstruo *m = &array_monstruos[id - 1];
                    			m->id = id;
                    			char* value_ptr = strstr(line, key);
                    			if (value_ptr) {
                        			value_ptr += strlen(key);
                        			if (strcmp(key, "HP") == 0) {
                            				sscanf(value_ptr, "%d", &m->vida);
                        			} else if (strcmp(key, "ATTACK_DAMAGE") == 0) {
                            				sscanf(value_ptr, "%d", &m->dano);
                        			} else if (strcmp(key, "VISION_RANGE") == 0) {
                            				sscanf(value_ptr, "%d", &m->rango_vision);
                        			} else if (strcmp(key, "ATTACK_RANGE") == 0) {
                        				sscanf(value_ptr, "%d", &m->rango_ataque);
                        			} else if (strcmp(key, "COORDS") == 0) {
                            				sscanf(value_ptr, "%d %d", &m->posicion.x, &m->posicion.y);
                        			}
                    			}
                		}
            		}
        	}
    	}
    	fclose(file);
}
