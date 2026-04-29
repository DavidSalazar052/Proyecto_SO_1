#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "truck.h"

void notificar_planificador(Camion* camion);
void ejecutar_fifo(Camion camiones[], int cantidad);
void ejecutar_round_robin(Camion camiones[], int cantidad, int quantum);

#endif
