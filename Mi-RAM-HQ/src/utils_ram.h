#ifndef MI_RAM_HQ_SRC_UTILS_RAM_H_
#define MI_RAM_HQ_SRC_UTILS_RAM_H_

#include <utils/hello.h>	// Incluyo la shared

typedef struct{
	uint32_t id_pcb_patota;
    uint32_t tam_path;
    void* path_tareas;
} t_pcb_patota;

typedef struct{
	uint32_t id_tcb_tripulante;
    t_estado estado;
    t_posicion posicion;
    uint32_t id_prox_instruc;
    uint32_t tam_path;
    char* path_tareas;
} t_tcb_tripulante;

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
