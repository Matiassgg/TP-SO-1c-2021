#ifndef I_MONGO_STORE_SRC_CONSOLA_MONGO_H_
#define I_MONGO_STORE_SRC_CONSOLA_MONGO_H_

#include "utils_mongo.h"
#include "msg_Mongo.h"

void leer_consola(void);
void procesar_mensajes_en_consola_mongo(char** palabras_del_mensaje);
void funcionFulera(int signal);

#endif /* I_MONGO_STORE_SRC_CONSOLA_MONGO_H_ */
