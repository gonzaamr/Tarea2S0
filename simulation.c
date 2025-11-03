#define _POSIX_C_SOURCE 199309L
#include "simulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <pthread.h>
#include <math.h>       
#include <time.h>      

static int g_game_over = 0;
static pthread_mutex_t g_game_mutex; 

typedef struct {
    Heroe* heroe;
    Grid* grid;
    Monstruo* monstruos;
    int num_monstruos;
} HeroThreadArgs;

typedef struct {
    Monstruo* self; 
    Heroe* heroe;
    Grid* grid;
    Monstruo* todos_los_monstruos;
    int num_monstruos;
} MonsterThreadArgs;

int calcular_distancia_manhattan(Coordenada a, Coordenada b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

void alertar_otros_monstruos(Monstruo* monstruo_alerta, Monstruo* todos, int num) {
    printf("¡Monstruo %d ALERTA a otros!\n", monstruo_alerta->id);
    
    for (int i = 0; i < num; i++) {
        Monstruo* otro_monstruo = &todos[i];
        
        if (otro_monstruo->id == monstruo_alerta->id) {
            continue;
        }
        int dist = calcular_distancia_manhattan(monstruo_alerta->posicion, otro_monstruo->posicion);
        
        if (dist <= monstruo_alerta->rango_vision) {
            pthread_mutex_lock(&otro_monstruo->mutex);
            if (otro_monstruo->estado == 0) {
                printf("  -> Monstruo %d despierta a Monstruo %d\n", monstruo_alerta->id, otro_monstruo->id);
                otro_monstruo->estado = 1; 
                pthread_cond_signal(&otro_monstruo->cond_despertar);
            }
            pthread_mutex_unlock(&otro_monstruo->mutex);
        }
    }
}

Monstruo* buscar_monstruo_en_rango(Heroe* heroe, Monstruo* monstruos, int num) {
    for (int i = 0; i < num; i++) {
        Monstruo* m = &monstruos[i];
        Coordenada pos_monstruo;
        int vida_monstruo;

        pthread_mutex_lock(&m->mutex);
        pos_monstruo = m->posicion;
        vida_monstruo = m->vida;
        pthread_mutex_unlock(&m->mutex);

        if (vida_monstruo > 0) {
            int dist = calcular_distancia_manhattan(heroe->posicion_actual, pos_monstruo);
            if (dist <= heroe->rango_ataque) {
                return m;
            }
        }
    }
    return NULL; 
}

void moverse_hacia_heroe(Monstruo* m, Coordenada pos_heroe) {
    pthread_mutex_lock(&m->mutex);

    int dx = pos_heroe.x - m->posicion.x;
    int dy = pos_heroe.y - m->posicion.y;

    if (abs(dx) > abs(dy)) {
        if (dx > 0) m->posicion.x++;
        else m->posicion.x--;
    } else if (dy != 0) {
        if (dy > 0) m->posicion.y++;
        else m->posicion.y--;
    }

    printf("Monstruo %d se mueve a (%d, %d)\n", m->id, m->posicion.x, m->posicion.y);
    pthread_mutex_unlock(&m->mutex);
}

void* hero_thread(void* arg) {
    HeroThreadArgs* args = (HeroThreadArgs*)arg;
    Heroe* heroe = args->heroe;

    while (1) {
        pthread_mutex_lock(&g_game_mutex);
        if (g_game_over) {
            pthread_mutex_unlock(&g_game_mutex);
            break; 
        }
        pthread_mutex_unlock(&g_game_mutex);
        
        pthread_mutex_lock(&heroe->mutex);
        if (heroe->vida <= 0) {
            pthread_mutex_unlock(&heroe->mutex);
            
            pthread_mutex_lock(&g_game_mutex);
            g_game_over = 2; 
            pthread_mutex_unlock(&g_game_mutex);
            break; 
        }
        pthread_mutex_unlock(&heroe->mutex);
        Monstruo* target = buscar_monstruo_en_rango(heroe, args->monstruos, args->num_monstruos);

        if (target != NULL) {
            printf("Héroe ataca a Monstruo %d\n", target->id);
            pthread_mutex_lock(&target->mutex);
            if (target->vida > 0) {
                target->vida -= heroe->dano;
                if (target->vida <= 0) {
                    printf("¡Monstruo %d ha muerto!\n", target->id);
                }
            }
            pthread_mutex_unlock(&target->mutex);

        } else {
            pthread_mutex_lock(&heroe->mutex); 
            if (heroe->indice_ruta_actual < heroe->largo_ruta) {
                heroe->posicion_actual = heroe->ruta[heroe->indice_ruta_actual];
                heroe->indice_ruta_actual++;
                printf("Héroe se mueve a (%d, %d)\n", heroe->posicion_actual.x, heroe->posicion_actual.y);

                if (heroe->indice_ruta_actual == heroe->largo_ruta) {
                    printf("¡Héroe llegó a la meta!\n");
                    pthread_mutex_lock(&g_game_mutex);
                    g_game_over = 1; 
                    pthread_mutex_unlock(&g_game_mutex);
                }
            }
            pthread_mutex_unlock(&heroe->mutex);
        }
        sleep(1); 
    }

    return NULL;
}

void* monster_thread(void* arg) {
    MonsterThreadArgs* args = (MonsterThreadArgs*)arg;
    Monstruo* self = args->self;
    Heroe* heroe = args->heroe;

    while (1) {
        pthread_mutex_lock(&g_game_mutex);
        if (g_game_over) {
            pthread_mutex_unlock(&g_game_mutex);
            break; 
        }
        pthread_mutex_unlock(&g_game_mutex);
        pthread_mutex_lock(&self->mutex);
        if (self->vida <= 0) {
            pthread_mutex_unlock(&self->mutex);
            break; 
        }
        if (self->estado == 0) {
            pthread_mutex_unlock(&self->mutex); 
            Coordenada pos_heroe;
            pthread_mutex_lock(&heroe->mutex);
            pos_heroe = heroe->posicion_actual;
            pthread_mutex_unlock(&heroe->mutex);
            
            int dist = calcular_distancia_manhattan(self->posicion, pos_heroe);

            if (dist <= self->rango_vision) {
                printf("¡Monstruo %d ve al Héroe!\n", self->id);
                pthread_mutex_lock(&self->mutex);
                self->estado = 1;
                pthread_mutex_unlock(&self->mutex);
                alertar_otros_monstruos(self, args->todos_los_monstruos, args->num_monstruos);
            
            } else {
                struct timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_sec += 1; 
                pthread_mutex_lock(&self->mutex);
                pthread_cond_timedwait(&self->cond_despertar, &self->mutex, &ts);
                pthread_mutex_unlock(&self->mutex);
            }

        } else {
            pthread_mutex_unlock(&self->mutex); 
            Coordenada pos_heroe;
            int vida_heroe;
            pthread_mutex_lock(&heroe->mutex);
            pos_heroe = heroe->posicion_actual;
            vida_heroe = heroe->vida;
            pthread_mutex_unlock(&heroe->mutex);

            if (vida_heroe <= 0) {
                continue; 
            }
            int dist = calcular_distancia_manhattan(self->posicion, pos_heroe);

            if (dist <= self->rango_ataque) {
                printf("¡Monstruo %d ataca al Héroe!\n", self->id);
                pthread_mutex_lock(&heroe->mutex);
                heroe->vida -= self->dano;
                if (heroe->vida <= 0) {
                    printf("¡El Héroe ha sido derrotado!\n");
                    pthread_mutex_lock(&g_game_mutex);
                    g_game_over = 2;
                    pthread_mutex_unlock(&g_game_mutex);
                }
                pthread_mutex_unlock(&heroe->mutex);
            } else {
                moverse_hacia_heroe(self, pos_heroe);
            }
            sleep(1); 
        }
    }

    return NULL;
}

void iniciar_simulacion(Grid* grid, Heroe* heroe, Monstruo* monstruos, int num_monstruos) {
    

    pthread_mutex_init(&g_game_mutex, NULL);
    g_game_over = 0; 

    HeroThreadArgs hero_args;
    hero_args.heroe = heroe;
    hero_args.grid = grid;
    hero_args.monstruos = monstruos;
    hero_args.num_monstruos = num_monstruos;

    MonsterThreadArgs* monster_args = malloc(sizeof(MonsterThreadArgs) * num_monstruos);
    if (monster_args == NULL) {
        perror("Error al crear args de monstruos");
        return;
    }

    for (int i = 0; i < num_monstruos; i++) {
        monster_args[i].self = &monstruos[i]; 
        monster_args[i].heroe = heroe;
        monster_args[i].grid = grid;
        monster_args[i].todos_los_monstruos = monstruos; 
        monster_args[i].num_monstruos = num_monstruos;
    }

    pthread_t* threads = malloc(sizeof(pthread_t) * (1 + num_monstruos));
    if (threads == NULL) {
        perror("Error al crear array de hilos");
        free(monster_args);
        return;
    }

    pthread_create(&threads[0], NULL, hero_thread, &hero_args);

    for (int i = 0; i < num_monstruos; i++) {
        pthread_create(&threads[i + 1], NULL, monster_thread, &monster_args[i]);
    }

    pthread_join(threads[0], NULL);
    
    for (int i = 0; i < num_monstruos; i++) {
        pthread_join(threads[i + 1], NULL);
    }


    if (g_game_over == 1) {
        printf("¡VICTORIA! El héroe ha llegado a la meta.\n");
    } else if (g_game_over == 2) {
        printf("DERROTA. El héroe ha muerto.\n");
    } else {
        printf("Simulación terminada por otra razón.\n");
    }

    free(monster_args);
    free(threads);
    pthread_mutex_destroy(&g_game_mutex);
}
