#include <stdio.h>
#include "queue.h"

void inicializar_cola(Cola* cola) {
    cola->frente = 0;
    cola->final = 0;
    cola->cantidad = 0;
}

int cola_vacia(const Cola* cola) {
    return cola->cantidad == 0;
}

int cola_llena(const Cola* cola) {
    return cola->cantidad == MAX_COLA;
}

int encolar(Cola* cola, Camion* camion) {
    if (cola_llena(cola)) {
        return 0;
    }

    cola->elementos[cola->final] = camion;
    cola->final = (cola->final + 1) % MAX_COLA;
    cola->cantidad++;

    return 1;
}

Camion* desencolar(Cola* cola) {
    Camion* camion;

    if (cola_vacia(cola)) {
        return NULL;
    }

    camion = cola->elementos[cola->frente];
    cola->frente = (cola->frente + 1) % MAX_COLA;
    cola->cantidad--;

    return camion;
}

void imprimir_cola(const Cola* cola) {
    int i;
    int posicion;

    printf("Cola LISTO: ");

    if (cola_vacia(cola)) {
        printf("vacia\n");
        return;
    }

    posicion = cola->frente;
    for (i = 0; i < cola->cantidad; i++) {
        printf("C%d ", cola->elementos[posicion]->id);
        posicion = (posicion + 1) % MAX_COLA;
    }

    printf("\n");
}
