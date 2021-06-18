#ifndef I_MONGO_STORE_SRC_UTILS_MONGO_H_
#define I_MONGO_STORE_SRC_UTILS_MONGO_H_

#include "../../utils/utils/utils.h"

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

uint32_t tiempo_sincronizacion;
t_list* posiciones_sabotaje;

t_config* config;
t_log* logger;
pthread_t hiloReceive;

uint32_t blocks;
uint32_t block_size;
char* ruta_bloques;
char* path_files;
char* path_bloques;
char* ruta_superbloque;
char* path_bitacoras;


void arrancar_servidor(void);
void serve_client(int*);
void procesar_mensaje_recibido(int, int);

typedef struct {
	u_int32_t id;
	uint32_t tamanio_tarea;
	char* tarea;
} tarea_Mongo;

typedef struct {
	char* stream;
	t_dictionary* diccionario;
} t_bloque_FS;

#endif /* I_MONGO_STORE_SRC_UTILS_MONGO_H_ */
