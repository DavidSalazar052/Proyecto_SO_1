#ifndef SYNC_H
#define SYNC_H

#include <pthread.h>
#include <semaphore.h>
#include "truck.h"

extern sem_t sem_muelles;
extern pthread_mutex_t mutex_log;
extern pthread_mutex_t mutex_estado;
extern pthread_mutex_t mutex_contador_muelles;
extern int muelles_ocupados;

void inicializar_sincronizacion(void);
void destruir_sincronizacion(void);
void escribir_log(const char* mensaje, Camion* camion);
void tomar_muelle(Camion* camion);
void liberar_muelle(Camion* camion);

#endif
