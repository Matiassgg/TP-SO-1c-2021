#ifndef MI_RAM_HQ_SRC_UTILS_RAM_H_
#define MI_RAM_HQ_SRC_UTILS_RAM_H_

#include <utils/hello.h>	// Incluyo la shared

char* ip_Mi_RAM_HQ;
char* puerto_escucha;
char* punto_montaje;
char* archivo_log;

t_config* config;
t_log* logger;
pthread_t hiloReceive;

void arrancar_servidor();
void serve_client(int* socketQueNosPasan);
void procesar_mensaje_recibido(int cod_op, int cliente_fd);

#endif /* MI_RAM_HQ_SRC_UTILS_RAM_H_ */
