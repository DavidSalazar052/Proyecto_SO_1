#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "truck.h"
#include "scheduler.h"
#include "metrics.h"
#include "utils.h"

static void cargar_camiones_prueba(Camion camiones[], int* cantidad) {
    int bursts[NUM_CAMIONES] = {7, 3, 5, 2, 6, 4, 8, 3};
    int prioridades[NUM_CAMIONES] = {2, 1, 3, 1, 2, 3, 2, 1};
    const char* cargas[NUM_CAMIONES] = {
        "Electrodomesticos",
        "Perecederos",
        "Materiales",
        "Perecederos",
        "Textiles",
        "Quimicos",
        "Maquinaria",
        "Perecederos"
    };
    int i;

    *cantidad = NUM_CAMIONES;

    for (i = 0; i < *cantidad; i++) {
        inicializar_camion(&camiones[i], i + 1, bursts[i], prioridades[i], cargas[i]);
    }
}

static void destruir_camiones(Camion camiones[], int cantidad) {
    int i;

    for (i = 0; i < cantidad; i++) {
        destruir_camion(&camiones[i]);
    }
}

static void mostrar_camiones_base(Camion camiones[], int cantidad) {
    int i;

    printf("\nCamiones de prueba:\n");
    printf("%-8s %-10s %-10s %-20s\n", "Camion", "Burst", "Prioridad", "Tipo carga");
    printf("----------------------------------------------------\n");

    for (i = 0; i < cantidad; i++) {
        printf("%-8d %-10d %-10d %-20s\n",
               camiones[i].id,
               camiones[i].burst_total,
               camiones[i].prioridad,
               camiones[i].tipo_carga);
    }
}

static void ejecutar_opcion_fifo(void) {
    Camion camiones[NUM_CAMIONES];
    int cantidad;

    cargar_camiones_prueba(camiones, &cantidad);
    mostrar_camiones_base(camiones, cantidad);
    ejecutar_fifo(camiones, cantidad);
    imprimir_resultados(camiones, cantidad, "FIFO");
    destruir_camiones(camiones, cantidad);
}

static void ejecutar_opcion_rr(void) {
    Camion camiones[NUM_CAMIONES];
    int cantidad;

    cargar_camiones_prueba(camiones, &cantidad);
    mostrar_camiones_base(camiones, cantidad);
    ejecutar_round_robin(camiones, cantidad, QUANTUM);
    imprimir_resultados(camiones, cantidad, "Round Robin");
    destruir_camiones(camiones, cantidad);
}

static void comparar_algoritmos(void) {
    Camion fifo[NUM_CAMIONES];
    Camion rr[NUM_CAMIONES];
    int cantidad_fifo;
    int cantidad_rr;
    double espera_fifo;
    double retorno_fifo;
    double espera_rr;
    double retorno_rr;

    cargar_camiones_prueba(fifo, &cantidad_fifo);
    cargar_camiones_prueba(rr, &cantidad_rr);

    mostrar_camiones_base(fifo, cantidad_fifo);

    ejecutar_fifo(fifo, cantidad_fifo);
    calcular_metricas(fifo, cantidad_fifo);

    ejecutar_round_robin(rr, cantidad_rr, QUANTUM);
    calcular_metricas(rr, cantidad_rr);

    espera_fifo = calcular_espera_promedio(fifo, cantidad_fifo);
    retorno_fifo = calcular_retorno_promedio(fifo, cantidad_fifo);
    espera_rr = calcular_espera_promedio(rr, cantidad_rr);
    retorno_rr = calcular_retorno_promedio(rr, cantidad_rr);

    printf("\n================ TABLA COMPARATIVA ================\n");
    printf("%-15s %-24s %-24s %-35s\n",
           "Algoritmo", "Espera promedio", "Retorno promedio", "Observaciones");
    printf("--------------------------------------------------------------------------------------------------\n");
    printf("%-15s %-24.2f %-24.2f %-35s\n",
           "FIFO", espera_fifo, retorno_fifo, "Puede sufrir efecto convoy");
    printf("%-15s %-24.2f %-24.2f %-35s\n",
           "Round Robin", espera_rr, retorno_rr, "Mas equitativo, mas cambios");
    printf("====================================================\n\n");

    destruir_camiones(fifo, cantidad_fifo);
    destruir_camiones(rr, cantidad_rr);
}

static void mostrar_menu(void) {
    printf("\n==============================================\n");
    printf(" SISTEMA DE GESTION DE TERMINAL DE CARGA\n");
    printf("==============================================\n");
    printf("1. Ejecutar FIFO\n");
    printf("2. Ejecutar Round Robin\n");
    printf("3. Comparar FIFO vs Round Robin\n");
    printf("4. Salir\n");
    printf("Seleccione una opcion: ");
}

int ejecutar_menu(void) {
    int opcion;

    do {
        mostrar_menu();

        if (scanf("%d", &opcion) != 1) {
            printf("Entrada invalida. Finalizando.\n");
            return EXIT_FAILURE;
        }

        switch (opcion) {
            case 1:
                ejecutar_opcion_fifo();
                break;
            case 2:
                ejecutar_opcion_rr();
                break;
            case 3:
                comparar_algoritmos();
                break;
            case 4:
                printf("Saliendo del simulador...\n");
                break;
            default:
                printf("Opcion invalida. Intente nuevamente.\n");
                break;
        }
    } while (opcion != 4);

    return EXIT_SUCCESS;
}