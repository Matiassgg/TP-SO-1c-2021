#ifndef I_MONGO_STORE_SRC_MSG_MONGO_H_
#define I_MONGO_STORE_SRC_MSG_MONGO_H_

#include "utils_mongo.h"

t_mover_hacia* deserializar_mover_hacia_posicion(uint32_t socket_cliente);
tarea_Mongo* desearilizar_ejecutar_tarea(uint32_t socket_cliente);


#endif /* I_MONGO_STORE_SRC_MSG_MONGO_H_ */
