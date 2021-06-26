#ifndef I_MONGO_STORE_SRC_MSG_MONGO_H_
#define I_MONGO_STORE_SRC_MSG_MONGO_H_

#include "utils_mongo.h"

//---------ENVIO DE MENSAJES--------//
void enviar_discordiador_sabotaje(t_posicion* posicion_sabotaje, uint32_t socket_discordiador);

//---------SERIALIZACION--------//
void serializar_sabotaje_en_mongo(t_posicion* posicion, t_buffer* buffer);


//---------DESERIALIZACION--------//
t_mover_hacia* deserializar_mover_hacia_posicion(uint32_t socket_cliente);
tarea_Mongo* deserializar_tarea(uint32_t socket_cliente);


//desearilizar_resolver_sabotaje(uint32_t socket_cliente);
//desearilizar_atender_sabotaje(uint32_t socket_cliente);


#endif /* I_MONGO_STORE_SRC_MSG_MONGO_H_ */
