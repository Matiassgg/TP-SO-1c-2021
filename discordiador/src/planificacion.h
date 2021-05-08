#ifndef DISCORDIADOR_SRC_PLANIFICACION_H_
#define DISCORDIADOR_SRC_PLANIFICACION_H_

#include "utils_discordiador.h"
#include "tripulantes.h"

void preparar_planificacion(void);
void planificar_patota(t_patota*);
void iniciar_planificacion();
t_pcb* crear_pcb(t_patota*);
t_list* agregar_tcbs(t_patota*);
void crear_colas_planificacion(void);
void liberar_pcb_patota(t_patota*);
void planificacion_segun_FIFO(void);

#endif /* DISCORDIADOR_SRC_PLANIFICACION_H_ */
