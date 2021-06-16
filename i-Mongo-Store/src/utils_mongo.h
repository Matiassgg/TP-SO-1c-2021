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

void arrancar_servidor(void);
void serve_client(int*);
void procesar_mensaje_recibido(int, int);

typedef struct {
	u_int32_t id;
	e_tarea tarea;
} tarea_Mongo;

#endif /* I_MONGO_STORE_SRC_UTILS_MONGO_H_ */
