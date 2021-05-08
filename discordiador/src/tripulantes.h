#ifndef DISCORDIADOR_SRC_TRIPULANTES_H_
#define DISCORDIADOR_SRC_TRIPULANTES_H_

#include "utils_discordiador.h"
#include "msg_discordiador.h"

t_tripulante* obtener_tripulante_de_patota(t_patota* patota, int i);
void iniciar_tripulante(t_tripulante* tripulante);
void hacer_tarea(t_tripulante* tripulante);
bool quedan_pasos(t_tripulante* tripulante);
bool esta_en_el_lugar(t_posicion posicion1, t_posicion posicion2);
bool quedan_movimientos(uint32_t posicion1, uint32_t posicion2);
t_movimiento avanzar_hacia(t_tripulante* tripulante, t_posicion posicion_meta);

#endif /* DISCORDIADOR_SRC_TRIPULANTES_H_ */
