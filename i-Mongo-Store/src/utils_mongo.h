#ifndef I_MONGO_STORE_SRC_UTILS_MONGO_H_
#define I_MONGO_STORE_SRC_UTILS_MONGO_H_

#include "../../utils/utils/utils.h"	// Incluyo la shared

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

t_config* config;
t_log* logger;
pthread_t hiloReceive;

void arrancar_servidor(void);
void serve_client(int*);
void procesar_mensaje_recibido(int, int);

typedef struct {
	u_int32_t id;
	t_posicion* posicion_origen;
	t_posicion* posicion_destino;
} mover_hacia;

#endif /* I_MONGO_STORE_SRC_UTILS_MONGO_H_ */
