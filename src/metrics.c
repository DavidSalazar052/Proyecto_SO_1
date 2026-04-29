#include <stdio.h>
#include <time.h>
#include "metrics.h"
#include "config.h"

static struct timespec inicio_simulacion;

void iniciar_reloj_simulacion(void) {
    clock_gettime(CLOCK_MONOTONIC, &inicio_simulacion);
}

double tiempo_actual(void) {
    struct timespec ahora;
    double segundos;
    double nanosegundos;

    clock_gettime(CLOCK_MONOTONIC, &ahora);

    segundos = (double)(ahora.tv_sec - inicio_simulacion.tv_sec);
    nanosegundos = (double)(ahora.tv_nsec - inicio_simulacion.tv_nsec) / 1000000000.0;

    return segundos + nanosegundos;
}

void calcular_metricas(Camion camiones[], int cantidad) {
    int i;

    for (i = 0; i < cantidad; i++) {
        camiones[i].tiempo_retorno = camiones[i].tiempo_fin - camiones[i].tiempo_llegada;

        camiones[i].tiempo_espera = camiones[i].tiempo_retorno -
                                    (camiones[i].burst_total * UNIDAD_TIEMPO_SEGUNDOS);

        if (camiones[i].tiempo_espera < 0) {
            camiones[i].tiempo_espera = 0;
        }
    }
}

double calcular_espera_promedio(Camion camiones[], int cantidad) {
    double suma = 0.0;
    int i;

    for (i = 0; i < cantidad; i++) {
        suma += camiones[i].tiempo_espera;
    }

    return suma / cantidad;
}

double calcular_retorno_promedio(Camion camiones[], int cantidad) {
    double suma = 0.0;
    int i;

    for (i = 0; i < cantidad; i++) {
        suma += camiones[i].tiempo_retorno;
    }

    return suma / cantidad;
}

void imprimir_resultados(Camion camiones[], int cantidad, const char* algoritmo) {
    int i;

    calcular_metricas(camiones, cantidad);

    printf("\n============================================================\n");
    printf("RESULTADOS DEL ALGORITMO: %s\n", algoritmo);
    printf("============================================================\n");
    printf("%-8s %-10s %-10s %-12s %-12s %-12s\n",
           "Camion", "Burst", "Prioridad", "Inicio", "Fin", "Retorno");
    printf("------------------------------------------------------------\n");

    for (i = 0; i < cantidad; i++) {
        printf("%-8d %-10d %-10d %-12.2f %-12.2f %-12.2f\n",
               camiones[i].id,
               camiones[i].burst_total,
               camiones[i].prioridad,
               camiones[i].tiempo_inicio,
               camiones[i].tiempo_fin,
               camiones[i].tiempo_retorno);
    }

    printf("------------------------------------------------------------\n");
    printf("Tiempo de espera promedio:  %.2f segundos\n",
           calcular_espera_promedio(camiones, cantidad));
    printf("Tiempo de retorno promedio: %.2f segundos\n",
           calcular_retorno_promedio(camiones, cantidad));
    printf("============================================================\n\n");
}