#ifndef DISCORDIADOR_SRC_MSG_DISCORDIADOR_H_
#define DISCORDIADOR_SRC_MSG_DISCORDIADOR_H_

#include "utils_discordiador.h"

//---------ENVIO DE MENSAJES--------//
void enviar_iniciar_patota(t_patota*, uint32_t);
void enviar_iniciar_tripulante(t_tripulante*, uint32_t);
void enviar_RAM_expulsar_tripulante(t_tripulante*, uint32_t);
void enviar_RAM_listar_tripulantes(t_tripulante*, uint32_t);

//---------SERIALIZACION--------//
void serializar_expulsar_tripulante(t_tripulante*, t_buffer*);
void serializar_iniciar_patota(t_patota*, t_buffer*);
void serializar_iniciar_tripulante(t_tripulante*, t_buffer*);
void serializar_listar_tripulantes(t_tripulante*, t_buffer*);

//---------DESERIALIZACION--------//
void listar_tripulantes_respuesta(uint32_t socket_cliente);	// TODO ??



#endif /* DISCORDIADOR_SRC_MSG_DISCORDIADOR_H_ */
