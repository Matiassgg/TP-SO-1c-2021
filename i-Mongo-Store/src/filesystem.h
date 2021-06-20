#ifndef I_MONGO_STORE_SRC_FILESYSTEM_H_
#define I_MONGO_STORE_SRC_FILESYSTEM_H_
#include "utils_mongo.h"

void inicializar_paths_aux();
void leer_superbloque(FILE* archivo);
void obtener_superbloque();
void inicializar_bloques();
bool directorio_existe(char* path);
bool archivo_existe(char* nombreArchivo);
bool archivo_recursos_existe(char* nombreArchivo);
char* obtener_path_files(char*);
int crear_archivo(char*);
int crear_archivo_recursos(char*);
int eliminar_archivo(char*);

t_bitarray* crear_bitmap(void);
#endif /* I_MONGO_STORE_SRC_FILESYSTEM_H_ */
