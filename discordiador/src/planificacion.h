#ifndef DISCORDIADOR_SRC_PLANIFICACION_H_
#define DISCORDIADOR_SRC_PLANIFICACION_H_

#include "utils_discordiador.h"

void iniciar_planificacion(void);
void planificar_patota(t_patota*);
t_pcb* crear_pcb(t_patota*);
t_list* crear_tcbs(t_patota*);
void crear_colas_planificacion(void);
void liberar_pcb_patota(t_patota*);

#endif /* DISCORDIADOR_SRC_PLANIFICACION_H_ */
