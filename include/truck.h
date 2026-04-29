#ifndef TRUCK_H
#define TRUCK_H

#include <pthread.h>

typedef enum {
    NUEVO,
    LISTO,
    EJECUCION,
    BLOQUEADO,
    TERMINADO
} Estado;

typedef struct {
    int id;
    int burst_total;
    int burst_restante;
    int prioridad;
    char tipo_carga[40];

    Estado estado;
    pthread_t hilo;

    pthread_mutex_t mutex;
    pthread_cond_t cond;

    int permiso_ejecucion;
    int quantum_asignado;
    int debe_salir;
    int evento_pendiente;

    double tiempo_llegada;
    double tiempo_inicio;
    double tiempo_fin;
    double tiempo_espera;
    double tiempo_retorno;
} Camion;

const char* estado_a_texto(Estado estado);
void inicializar_camion(Camion* camion, int id, int burst, int prioridad, const char* tipo_carga);
void destruir_camion(Camion* camion);
void cambiar_estado(Camion* camion, Estado nuevo_estado);
void despachar_camion(Camion* camion, int quantum);
void* ejecutar_camion(void* arg);

#endif
