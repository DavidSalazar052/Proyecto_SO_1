#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include "sync.h"
#include "config.h"
#include "truck.h"

sem_t sem_muelles;
pthread_mutex_t mutex_log;
pthread_mutex_t mutex_estado;
pthread_mutex_t mutex_contador_muelles;
int muelles_ocupados = 0;

void inicializar_sincronizacion(void) {
    FILE* archivo;

    mkdir("logs", 0777);

    archivo = fopen(LOG_FILE, "w");
    if (archivo == NULL) {
        perror("No se pudo crear el archivo de log");
        exit(EXIT_FAILURE);
    }
    fprintf(archivo, "===== LOG DE OPERACIONES DE LA TERMINAL =====\n");
    fclose(archivo);

    if (sem_init(&sem_muelles, 0, NUM_MUELLES) != 0) {
        perror("Error al inicializar semaforo de muelles");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&mutex_log, NULL);
    pthread_mutex_init(&mutex_estado, NULL);
    pthread_mutex_init(&mutex_contador_muelles, NULL);

    muelles_ocupados = 0;
}

void destruir_sincronizacion(void) {
    sem_destroy(&sem_muelles);
    pthread_mutex_destroy(&mutex_log);
    pthread_mutex_destroy(&mutex_estado);
    pthread_mutex_destroy(&mutex_contador_muelles);
}

void escribir_log(const char* mensaje, Camion* camion) {
    FILE* archivo;
    int bloqueo_log;
    Estado estado_anterior = NUEVO;

    bloqueo_log = pthread_mutex_trylock(&mutex_log);

    if (bloqueo_log != 0) {
        if (camion != NULL) {
            pthread_mutex_lock(&mutex_estado);
            estado_anterior = camion->estado;
            camion->estado = BLOQUEADO;
            pthread_mutex_unlock(&mutex_estado);
        }

        pthread_mutex_lock(&mutex_log);

        if (camion != NULL) {
            archivo = fopen(LOG_FILE, "a");
            if (archivo != NULL) {
                fprintf(archivo, "[Camion %02d] Estado: BLOQUEADO - esperando mutex del log\n", camion->id);
                fclose(archivo);
            }

            printf("[Camion %02d] Estado: BLOQUEADO - esperando mutex del log\n", camion->id);

            pthread_mutex_lock(&mutex_estado);
            camion->estado = estado_anterior;
            pthread_mutex_unlock(&mutex_estado);
        }
    }

    archivo = fopen(LOG_FILE, "a");
    if (archivo != NULL) {
        fprintf(archivo, "%s\n", mensaje);
        fclose(archivo);
    }

    printf("%s\n", mensaje);

    pthread_mutex_unlock(&mutex_log);
}

void tomar_muelle(Camion* camion) {
    char mensaje[160];

    snprintf(mensaje, sizeof(mensaje),
             "[Camion %02d] Esperando un muelle disponible...", camion->id);
    escribir_log(mensaje, camion);

    sem_wait(&sem_muelles);

    pthread_mutex_lock(&mutex_contador_muelles);
    muelles_ocupados++;
    snprintf(mensaje, sizeof(mensaje),
             "[Camion %02d] Tomo muelle. Ocupados: %d/%d",
             camion->id, muelles_ocupados, NUM_MUELLES);
    pthread_mutex_unlock(&mutex_contador_muelles);

    escribir_log(mensaje, camion);
}

void liberar_muelle(Camion* camion) {
    char mensaje[160];

    pthread_mutex_lock(&mutex_contador_muelles);
    muelles_ocupados--;
    snprintf(mensaje, sizeof(mensaje),
             "[Camion %02d] Libero muelle. Ocupados: %d/%d",
             camion->id, muelles_ocupados, NUM_MUELLES);
    pthread_mutex_unlock(&mutex_contador_muelles);

    escribir_log(mensaje, camion);
    sem_post(&sem_muelles);
}
