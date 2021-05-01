#ifndef DISCORDIADOR_SRC_UTILS_DISCORDIADOR_H_
#define DISCORDIADOR_SRC_UTILS_DISCORDIADOR_H_

#include <utils/hello.h>

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

char* algoritmo;
uint32_t quantum;

t_config* config;
t_log* logger;
pthread_t hiloReceive;

t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_exec;
t_queue* cola_exit;
// Hay 2 tipos de colas de bloqueados: E/S y Sabotaje o Emergencia
t_queue* cola_bloq_E_S;
t_queue* cola_bloq_Emergencia;

uint32_t cantidad_patotas;
//t_list* patotas; VER SI ES NECESARIO


void arrancar_servidor();
void serve_client(int* socketQuenosPAsan);
void procesar_mensaje_recibido(int cod_op, int cliente_fd);

#endif /* DISCORDIADOR_SRC_UTILS_DISCORDIADOR_H_ */
