#ifndef DISCORDIADOR_SRC_PLANIFICACION_H_
#define DISCORDIADOR_SRC_PLANIFICACION_H_

#include "utils_discordiador.h"
#include "tripulantes.h"

void preparar_planificacion(void);
void planificar_patota(t_patota*);
void arrancar_planificacion();
void iniciar_planificacion(void);
void pausar_planificacion(void);
//t_pcb* crear_pcb(t_patota*);
//t_list* crear_tcbs(t_patota*);
void crear_colas_planificacion(void);
//void liberar_pcb_patota(t_patota*);
bool verificar_planificacion_activa();
void planificacion_segun_FIFO(void);
void planificacion_segun_RR(void);
void finalizar_tripulante_plani(uint32_t);

#endif /* DISCORDIADOR_SRC_PLANIFICACION_H_ */
