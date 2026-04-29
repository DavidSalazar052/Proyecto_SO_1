#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "scheduler.h"
#include "queue.h"
#include "sync.h"
#include "metrics.h"
#include "config.h"

static pthread_mutex_t mutex_planificador = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_planificador = PTHREAD_COND_INITIALIZER;
static int eventos_pendientes = 0;

static void reiniciar_eventos(Camion camiones[], int cantidad) {
    int i;

    pthread_mutex_lock(&mutex_planificador);
    eventos_pendientes = 0;

    for (i = 0; i < cantidad; i++) {
        camiones[i].evento_pendiente = 0;
    }

    pthread_mutex_unlock(&mutex_planificador);
}

void notificar_planificador(Camion* camion) {
    pthread_mutex_lock(&mutex_planificador);
    camion->evento_pendiente = 1;
    eventos_pendientes++;
    pthread_cond_signal(&cond_planificador);
    pthread_mutex_unlock(&mutex_planificador);
}

static Camion* esperar_evento(Camion camiones[], int cantidad) {
    int i;
    Camion* camion = NULL;

    pthread_mutex_lock(&mutex_planificador);

    while (eventos_pendientes == 0) {
        pthread_cond_wait(&cond_planificador, &mutex_planificador);
    }

    for (i = 0; i < cantidad; i++) {
        if (camiones[i].evento_pendiente) {
            camiones[i].evento_pendiente = 0;
            eventos_pendientes--;
            camion = &camiones[i];
            break;
        }
    }

    pthread_mutex_unlock(&mutex_planificador);

    return camion;
}

static void crear_hilos_camiones(Camion camiones[], int cantidad) {
    int i;

    for (i = 0; i < cantidad; i++) {
        if (pthread_create(&camiones[i].hilo, NULL, ejecutar_camion, &camiones[i]) != 0) {
            perror("Error al crear hilo de camion");
            exit(EXIT_FAILURE);
        }
    }
}

static void unir_hilos_camiones(Camion camiones[], int cantidad) {
    int i;

    for (i = 0; i < cantidad; i++) {
        pthread_join(camiones[i].hilo, NULL);
    }
}

static int camion_terminado(Camion* camion) {
    int terminado;

    pthread_mutex_lock(&camion->mutex);
    terminado = camion->burst_restante <= 0;
    pthread_mutex_unlock(&camion->mutex);

    return terminado;
}

static int burst_restante(Camion* camion) {
    int restante;

    pthread_mutex_lock(&camion->mutex);
    restante = camion->burst_restante;
    pthread_mutex_unlock(&camion->mutex);

    return restante;
}

void ejecutar_fifo(Camion camiones[], int cantidad) {
    Cola cola;
    int i;
    int activos = 0;
    int terminados = 0;

    printf("\n===== Ejecutando FIFO =====\n");

    inicializar_sincronizacion();
    iniciar_reloj_simulacion();
    reiniciar_eventos(camiones, cantidad);
    inicializar_cola(&cola);

    crear_hilos_camiones(camiones, cantidad);

    for (i = 0; i < cantidad; i++) {
        camiones[i].tiempo_llegada = tiempo_actual();
        cambiar_estado(&camiones[i], LISTO);
        encolar(&cola, &camiones[i]);
    }

    while (terminados < cantidad) {
        while (activos < NUM_MUELLES && !cola_vacia(&cola)) {
            Camion* camion = desencolar(&cola);
            int restante = burst_restante(camion);
            despachar_camion(camion, restante);
            activos++;
        }

        if (activos > 0) {
            Camion* camion = esperar_evento(camiones, cantidad);
            activos--;

            if (camion_terminado(camion)) {
                terminados++;
            }
        }
    }

    unir_hilos_camiones(camiones, cantidad);
    destruir_sincronizacion();
}

void ejecutar_round_robin(Camion camiones[], int cantidad, int quantum) {
    Cola cola;
    int i;
    int activos = 0;
    int terminados = 0;

    printf("\n===== Ejecutando Round Robin =====\n");

    inicializar_sincronizacion();
    iniciar_reloj_simulacion();
    reiniciar_eventos(camiones, cantidad);
    inicializar_cola(&cola);

    crear_hilos_camiones(camiones, cantidad);

    for (i = 0; i < cantidad; i++) {
        camiones[i].tiempo_llegada = tiempo_actual();
        cambiar_estado(&camiones[i], LISTO);
        encolar(&cola, &camiones[i]);
    }

    while (terminados < cantidad) {
        while (activos < NUM_MUELLES && !cola_vacia(&cola)) {
            Camion* camion = desencolar(&cola);
            int restante = burst_restante(camion);
            int q = quantum;

            if (q > restante) {
                q = restante;
            }

            despachar_camion(camion, q);
            activos++;
        }

        if (activos > 0) {
            Camion* camion = esperar_evento(camiones, cantidad);
            activos--;

            if (camion_terminado(camion)) {
                terminados++;
            } else {
                encolar(&cola, camion);
            }
        }
    }

    unir_hilos_camiones(camiones, cantidad);
    destruir_sincronizacion();
}