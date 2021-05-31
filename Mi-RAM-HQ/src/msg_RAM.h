#ifndef MI_RAM_HQ_SRC_MSG_RAM_H_
#define MI_RAM_HQ_SRC_MSG_RAM_H_

#include "utils_ram.h"

t_tripulante* deserializar_expulsar_tripulante(uint32_t socket_cliente);
t_tripulante* deserializar_solicitar_tarea(uint32_t socket_cliente);
void enviar_solicitar_tarea_respuesta(t_tarea* tarea, uint32_t socket_cliente);
void serializar_solicitar_tarea_respuesta(t_tarea* msg, t_buffer* buffer);

#endif /* MI_RAM_HQ_SRC_MSG_RAM_H_ */
