#ifndef I_MONGO_STORE_SRC_SABOTAJE_H_
#define I_MONGO_STORE_SRC_SABOTAJE_H_

#include "utils_mongo.h"
#include "dirent.h"

void resolver_sabotaje_files_blocks(char*);
void resolver_sabotaje_files_blockcount(char*);
char* obtener_path_bitacora(char* nombreBitacora);
FILE* abrirSuperbloque(char* modo);
t_list* obtener_bloques_usados();
t_list* obtener_bloques_bitacora();
t_list* obtener_bloques_recursos();
void resolver_sabotaje_superbloque_blocks();
void resolver_sabotaje_superbloque_bitmap(t_bitarray* bitarray, t_list* bloques_usados);
void detectar_sabotaje_superbloque_bitmap();
void resolver_sabotaje_files_size(char* archivo);
void actualizar_valor_blocks_superbloque(uint32_t blocksReales);
t_list* listaArchivosDeBitacora();
char* obtener_path_bitacora(char* nombreBitacora);

#endif /* I_MONGO_STORE_SRC_SABOTAJE_H_ */
