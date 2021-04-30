#ifndef DISCORDIADOR_SRC_MSG_DISCORDIADOR_H_
#define DISCORDIADOR_SRC_MSG_DISCORDIADOR_H_

#include "utils_discordiador.h"

void enviar_iniciar_patota(iniciar_patota* msg, uint32_t socketConexion);
void serializar_iniciar_patota(iniciar_patota* msg, t_buffer* buffer);

#endif /* DISCORDIADOR_SRC_MSG_DISCORDIADOR_H_ */
