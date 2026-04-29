#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "truck.h"
#include "sync.h"
#include "scheduler.h"
#include "metrics.h"
#include "config.h"

const char* estado_a_texto(Estado estado) {
    switch (estado) {
        case NUEVO:
            return "NUEVO";
        case LISTO:
            return "LISTO";
        case EJECUCION:
            return "EJECUCION";
        case BLOQUEADO:
            return "BLOQUEADO";
        case TERMINADO:
            return "TERMINADO";
        default:
            return "DESCONOCIDO";
    }
}

void inicializar_camion(Camion* camion, int id, int burst, int prioridad, const char* tipo_carga) {
    camion->id = id;
    camion->burst_total = burst;
    camion->burst_restante = burst;
    camion->prioridad = prioridad;

    strncpy(camion->tipo_carga, tipo_carga, sizeof(camion->tipo_carga) - 1);
    camion->tipo_carga[sizeof(camion->tipo_carga) - 1] = '\0';

    camion->estado = NUEVO;
    camion->permiso_ejecucion = 0;
    camion->quantum_asignado = 0;
    camion->debe_salir = 0;
    camion->evento_pendiente = 0;

    camion->tiempo_llegada = 0.0;
    camion->tiempo_inicio = -1.0;
    camion->tiempo_fin = 0.0;
    camion->tiempo_espera = 0.0;
    camion->tiempo_retorno = 0.0;

    pthread_mutex_init(&camion->mutex, NULL);
    pthread_cond_init(&camion->cond, NULL);
}

void destruir_camion(Camion* camion) {
    pthread_mutex_destroy(&camion->mutex);
    pthread_cond_destroy(&camion->cond);
}

void cambiar_estado(Camion* camion, Estado nuevo_estado) {
    char mensaje[180];

    pthread_mutex_lock(&mutex_estado);
    camion->estado = nuevo_estado;
    pthread_mutex_unlock(&mutex_estado);

    snprintf(mensaje, sizeof(mensaje),
             "[Camion %02d] Estado: %s", camion->id, estado_a_texto(nuevo_estado));
    escribir_log(mensaje, camion);
}

void despachar_camion(Camion* camion, int quantum) {
    char mensaje[180];

    pthread_mutex_lock(&camion->mutex);
    camion->quantum_asignado = quantum;
    camion->permiso_ejecucion = 1;
    pthread_cond_signal(&camion->cond);
    pthread_mutex_unlock(&camion->mutex);

    snprintf(mensaje, sizeof(mensaje),
             "[Planificador] Despacha Camion %02d con quantum %d",
             camion->id, quantum);
    escribir_log(mensaje, camion);
}

void* ejecutar_camion(void* arg) {
    Camion* camion = (Camion*) arg;

    cambiar_estado(camion, NUEVO);

    while (1) {
        int quantum;
        int trabajo;
        int finalizado;
        char mensaje[180];

        pthread_mutex_lock(&camion->mutex);
        while (!camion->permiso_ejecucion && !camion->debe_salir) {
            pthread_cond_wait(&camion->cond, &camion->mutex);
        }

        if (camion->debe_salir) {
            pthread_mutex_unlock(&camion->mutex);
            break;
        }

        quantum = camion->quantum_asignado;
        camion->permiso_ejecucion = 0;
        pthread_mutex_unlock(&camion->mutex);

        cambiar_estado(camion, BLOQUEADO);
        tomar_muelle(camion);

        pthread_mutex_lock(&camion->mutex);
        if (camion->tiempo_inicio < 0) {
            camion->tiempo_inicio = tiempo_actual();
        }

        trabajo = quantum;
        if (trabajo > camion->burst_restante) {
            trabajo = camion->burst_restante;
        }
        pthread_mutex_unlock(&camion->mutex);

        cambiar_estado(camion, EJECUCION);

        snprintf(mensaje, sizeof(mensaje),
                 "[Camion %02d] Cargando %d unidad(es). Carga restante antes: %d",
                 camion->id, trabajo, camion->burst_restante);
        escribir_log(mensaje, camion);

        usleep(trabajo * UNIDAD_TIEMPO_US);

        pthread_mutex_lock(&camion->mutex);
        camion->burst_restante -= trabajo;
        finalizado = camion->burst_restante <= 0;

        if (finalizado) {
            camion->burst_restante = 0;
            camion->tiempo_fin = tiempo_actual();
        }
        pthread_mutex_unlock(&camion->mutex);

        liberar_muelle(camion);

        if (finalizado) {
            cambiar_estado(camion, TERMINADO);
        } else {
            cambiar_estado(camion, LISTO);
        }

        notificar_planificador(camion);

        if (finalizado) {
            break;
        }
    }

    return NULL;
}