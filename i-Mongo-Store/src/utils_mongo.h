#ifndef I_MONGO_STORE_SRC_UTILS_MONGO_H_
#define I_MONGO_STORE_SRC_UTILS_MONGO_H_

#include "../../utils/utils/utils.h"
#include "filesystem.h"
#include "msg_Mongo.h"

char* ip_Mongo_Store;
char* puerto_escucha;

char* ip_Mi_RAM_HQ;
char* puerto_Mi_RAM_HQ;
uint32_t socket_Mi_RAM_HQ;

char* ip_discordiador;
char* puerto_discordiador;
uint32_t socket_discordiador;

char* punto_montaje;
char* archivo_log;
char* path_superbloque;
char* bitarrayFS;
char* ruta_bitmap;

uint32_t tiempo_sincronizacion;
t_list* posiciones_sabotaje;

t_config* config;
t_log* logger;
pthread_t hiloReceive;

uint32_t blocks;
uint32_t block_size;
t_bitarray* bitmap;

char* ruta_bloques;
char* path_files;
char* path_bloques;
char* ruta_superbloque;
char* path_bitacoras;
char* bitarray_string;


void arrancar_servidor(void);
void serve_client(int*);
void procesar_mensaje_recibido(int, int);
void verificar_archivo_tarea(t_tarea*);
void procesar_tarea(t_tarea*);
void informar_falta_archivo(t_tarea*, char*);
void procesar_falta_archivo(t_tarea*,char*);
uint32_t cantidad_caracteres_archivo(char, char*);
void eliminar_caracteres_llenado_a_archivo(char, uint32_t, char*);
void eliminar_caracteres_llenado_segun_tarea(char*, uint32_t );
void agregar_caracteres_llenado_a_archivo(char, uint32_t, char*);
void agregar_caracteres_llenado_segun_tarea(char*, uint32_t);

#endif /* I_MONGO_STORE_SRC_UTILS_MONGO_H_ */
