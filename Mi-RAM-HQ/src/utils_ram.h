#ifndef MI_RAM_HQ_SRC_UTILS_RAM_H_
#define MI_RAM_HQ_SRC_UTILS_RAM_H_

#include "../../utils/utils/utils.h"
#include "msg_RAM.h"
#include "mapa.h"


//---------CONFIG--------//
char* ip_Mi_RAM_HQ;
char* puerto_escucha;
char* punto_montaje;
char* archivo_log;
uint32_t tamanio_memoria;
char* esquema_memoria;
uint32_t tamanio_pagina;
uint32_t tamanio_swap;
char* path_swap;
char* algoritmo_reemplazo;

//---------Segmentación pura--------//
// Se vera :v

typedef struct {
	uint32_t nro_segmento;
	uint32_t inicio;
	uint32_t tamanio;

} t_segmento;

//---------Paginación simple--------//
typedef enum {
	LIBRE,
	OCUPADO
} t_estado_marco;

typedef struct {
	uint32_t nro_pagina;
	uint32_t nro_marco;
	t_estado_marco estado;
	uint32_t tamanio;
} t_tabla_paginas;


t_config* config;
t_log* logger;
pthread_t hiloReceive;

void* memoria;
FILE* espacio_swap;

void arrancar_servidor(void);
void serve_client(int*);
void procesar_mensaje_recibido(int, int);

#endif /* MI_RAM_HQ_SRC_UTILS_RAM_H_ */
