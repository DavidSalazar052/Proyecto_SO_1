#ifndef CONFIG_H
#define CONFIG_H

#define NUM_MUELLES 3
#define NUM_CAMIONES 8
#define MAX_COLA 100
#define QUANTUM 2

/* Para que la simulacion no dure demasiado en clase.
   1 unidad de burst = 0.25 segundos reales. */
#define UNIDAD_TIEMPO_US 250000
#define UNIDAD_TIEMPO_SEGUNDOS 0.25

#define LOG_FILE "logs/operaciones.log"

#endif
