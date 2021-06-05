#ifndef MI_RAM_HQ_SRC_UTILS_RAM_H_
#define MI_RAM_HQ_SRC_UTILS_RAM_H_

#include "../../utils/utils/utils.h"
#include "msg_RAM.h"
#include "mapa.h"


//---------FUNCIONES--------//
void* seleccionar_victima;
//-------------------------//

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

void* memoria;
FILE* espacio_swap;

//

typedef struct {
	uint32_t indice;
	void* marco;
	bool libre;
	bool bitUso;
	bool bitModificado;
	char* timeStamp;
	int idPatota;
	char* discordiador;
} entradaTablaMarcos;

typedef struct {
	int indiceMarcoSwap;
	bool libre;
	int idPatota;
	// char* nombreTripulante;
} entradaSwap;

//---------MAIN--------//
t_config* config; //1
t_log* logger; //1
uint32_t socketConexionApp;//1
void* memoria;
FILE* espacio_swap;
t_list* entradas_swap;
uint32_t tamanio_pagina;
t_dictionary* dic_discordiador_tabla_segmentos;
// uint32_t tamanio_nombre_plato_maximo;
t_list* patotasCreadas;
entradaTablaMarcos* punteroMarcoClock;
t_list* tablaDeMarcos;
t_list* tareas;
char* path_tareas;
//-------------------------//

//Semaforos
pthread_mutex_t mutexTablaMarcos;
pthread_mutex_t mutexEntradasSwap;
pthread_mutex_t mutexMemoria;
pthread_mutex_t mutexSwap;
pthread_mutex_t mutexTablaSegmentos;
pthread_mutex_t mutexInfoPatotaAEnviar;
pthread_mutex_t mutexEscribirMemoria;
pthread_mutex_t mutexEliminarDeMemoria;
pthread_mutex_t mutexPatotasCreadas;
pthread_mutex_t mutexFree;
pthread_mutex_t mutexVictima;
pthread_mutex_t mutexBuscarSegmento;
pthread_mutex_t mutexBuscarPagina;
pthread_mutex_t mutexBuscarInfoTripulante;
pthread_mutex_t mutexBuscarSwap;

pthread_t hiloReceive;

//Server

void arrancar_servidor(void);
void serve_client(int*);
void procesar_mensaje_recibido(int, int);
t_tarea* obtener_tarea(t_tripulante* tripulante);
void obtener_tareas(t_pcb* pcb);
t_tarea* obtener_tarea_archivo(char* tarea_string);
e_tarea obtener_nombre_tarea(char* tarea);

#endif /* MI_RAM_HQ_SRC_UTILS_RAM_H_ */
