#ifndef PROJECT_SRC_MAIN_H_
#define PROJECT_SRC_MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>	// Incluyo la shared

char* ip_RAM;
char* puerto;
char* punto_montaje;
char* archivo_log;

t_config* config;
t_log* logger;
pthread_t hiloReceive;

void leer_config(void);
void arrancar_servidor();
void serve_client(int* socketQuenosPAsan);
void procesar_mensaje_recibido(int cod_op, int cliente_fd);



#endif /* PROJECT_SRC_MAIN_H_ */
