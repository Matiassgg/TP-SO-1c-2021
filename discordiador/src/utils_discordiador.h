#ifndef DISCORDIADOR_SRC_UTILS_DISCORDIADOR_H_
#define DISCORDIADOR_SRC_UTILS_DISCORDIADOR_H_

#include <utils/hello.h>	// Incluyo la shared

char* ip_discordiador;
char* puerto_escucha;

char* ip_Mi_RAM_HQ;
char* puerto_Mi_RAM_HQ;
int socket_Mi_RAM_HQo;

char* ip_Mongo_Store;
char* puerto_Mongo_Store;
int socket_Mongo_Store;

char* punto_montaje;
char* archivo_log;

t_config* config;
t_log* logger;
pthread_t hiloReceive;

void arrancar_servidor();
void serve_client(int* socketQuenosPAsan);
void procesar_mensaje_recibido(int cod_op, int cliente_fd);

#endif /* DISCORDIADOR_SRC_UTILS_DISCORDIADOR_H_ */
