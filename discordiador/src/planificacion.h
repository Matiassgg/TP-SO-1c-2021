#ifndef DISCORDIADOR_SRC_PLANIFICACION_H_
#define DISCORDIADOR_SRC_PLANIFICACION_H_

#include "utils_discordiador.h"

void iniciar_planificacion(void);
void planificar_patota(t_patota* patota);
t_pcb* crear_pcb(t_patota* patota);
t_list* crear_tcbs(t_patota* patota);

#endif /* DISCORDIADOR_SRC_PLANIFICACION_H_ */
