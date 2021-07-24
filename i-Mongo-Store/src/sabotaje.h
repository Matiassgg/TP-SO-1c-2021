#ifndef I_MONGO_STORE_SRC_SABOTAJE_H_
#define I_MONGO_STORE_SRC_SABOTAJE_H_

#include "utils_mongo.h"
#include "dirent.h"

void resolver_sabotaje_files_blocks(char*);
void resolver_sabotaje_files_blockcount(char*);
char* obtener_path_bitacora(char* nombreBitacora);

#endif /* I_MONGO_STORE_SRC_SABOTAJE_H_ */
