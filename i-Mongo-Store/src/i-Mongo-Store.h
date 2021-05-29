#ifndef I_MONGO_STORE_SRC_I_MONGO_STORE_H_
#define I_MONGO_STORE_SRC_I_MONGO_STORE_H_

#include "utils_mongo.h"
#include "consola_mongo.h"

#define ARCHIVO_CONFIG "../i-Mongo-Store.config"

void iniciar_mongo(void);
void leer_config(void);
void FS_RESET();
void crear_punto_de_montaje();

#endif /* I_MONGO_STORE_SRC_I_MONGO_STORE_H_ */
