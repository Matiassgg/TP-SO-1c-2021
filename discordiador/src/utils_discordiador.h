#ifndef DISCORDIADOR_SRC_UTILS_DISCORDIADOR_H_
#define DISCORDIADOR_SRC_UTILS_DISCORDIADOR_H_

#include "../../utils/utils/utils.h"

char* ip_discordiador;
char* puerto_escucha;
char* ip_Mi_RAM_HQ;
char* puerto_Mi_RAM_HQ;
uint32_t socket_Mi_RAM_HQ;

char* ip_Mongo_Store;
char* puerto_Mongo_Store;
uint32_t socket_Mongo_Store;

char* punto_montaje;
char* archivo_log;

uint32_t grado_multitarea;
uint32_t duracion_sabotaje;
uint32_t retardo_ciclo_cpu;

void* planificacion_corto_plazo;
char* algoritmo;
uint32_t quantum;
bool planificacion_habilitada;

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


pthread_mutex_t mutex_cola_ready;
sem_t semaforo_planificacion;

uint32_t cantidad_patotas;
//t_list* patotas;
uint32_t id_tcb;
uint32_t cantidad_tripulantes;
void arrancar_servidor(void);
void serve_client(int*);
void procesar_mensaje_recibido(int, int);

#endif /* DISCORDIADOR_SRC_UTILS_DISCORDIADOR_H_ */
