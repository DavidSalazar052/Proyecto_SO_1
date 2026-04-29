#ifndef METRICS_H
#define METRICS_H

#include "truck.h"

void iniciar_reloj_simulacion(void);
double tiempo_actual(void);
void calcular_metricas(Camion camiones[], int cantidad);
double calcular_espera_promedio(Camion camiones[], int cantidad);
double calcular_retorno_promedio(Camion camiones[], int cantidad);
void imprimir_resultados(Camion camiones[], int cantidad, const char* algoritmo);

#endif
