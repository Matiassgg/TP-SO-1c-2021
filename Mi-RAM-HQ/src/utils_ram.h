#ifndef MI_RAM_HQ_SRC_UTILS_RAM_H_
#define MI_RAM_HQ_SRC_UTILS_RAM_H_

#include "../../utils/utils/utils.h"	// Incluyo la shared

char* ip_Mi_RAM_HQ;
char* puerto_escucha;
char* punto_montaje;
char* archivo_log;

t_config* config;
t_log* logger;
pthread_t hiloReceive;

void arrancar_servidor(void);
void serve_client(int*);
void procesar_mensaje_recibido(int, int);

#endif /* MI_RAM_HQ_SRC_UTILS_RAM_H_ */
