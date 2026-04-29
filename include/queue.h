#ifndef QUEUE_H
#define QUEUE_H

#include "truck.h"
#include "config.h"

typedef struct {
    Camion* elementos[MAX_COLA];
    int frente;
    int final;
    int cantidad;
} Cola;

void inicializar_cola(Cola* cola);
int cola_vacia(const Cola* cola);
int cola_llena(const Cola* cola);
int encolar(Cola* cola, Camion* camion);
Camion* desencolar(Cola* cola);
void imprimir_cola(const Cola* cola);

#endif
