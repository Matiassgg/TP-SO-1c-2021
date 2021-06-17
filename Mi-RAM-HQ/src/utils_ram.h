#ifndef MI_RAM_HQ_SRC_UTILS_RAM_H_
#define MI_RAM_HQ_SRC_UTILS_RAM_H_

#include "../../utils/utils/utils.h"
#include "msg_RAM.h"
#include "mapa.h"
#include "signal.h"
#include "unistd.h"


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
char* esquema_memoria;
char* criterio_seleccion;

typedef enum {
	TAREAS,
	PCB,
	TCB
} e_tipo_dato;

//---------Segmentación pura--------//
// Se vera :v

typedef struct {
	uint32_t nro_segmento;
	uint32_t inicio;
	uint32_t tamanio;
} t_segmento;

typedef struct {
	t_segmento segmento;
	uint32_t id_tripulante;
} t_segmento_tcb;

typedef struct {
	t_segmento* segmento_pcb;
	t_segmento* segmento_tareas;
	t_list* segmentos_tripulantes;
	uint32_t id_patota_asociada;
	uint32_t tareas_dadas;
	uint32_t cant_segmentos;
	pthread_mutex_t mutex_obtener_tareas;
} t_tabla_segmentos;

typedef struct {
	uint32_t nro_segmento;
	uint32_t id_asociado;
	e_tipo_dato tipo_dato;
} t_asociador_segmento;

//---------Paginación simple--------//
typedef enum {
	LIBRE,
	OCUPADO
} t_estado_marco;

typedef struct {
	t_list* paginas;
	uint32_t id_patota_asociada;
	uint32_t tareas_dadas;
} t_tabla_paginas;

typedef struct {
	uint32_t nro_pagina;
	uint32_t id_asociado;
	e_tipo_dato tipo_dato;
} t_asociador_pagina;

typedef struct {
	uint32_t numeroPagina;
	t_asociador_pagina* asociador;
	uint32_t numeroMarco;
} t_pagina;


//

typedef struct {
	uint32_t indice;
	void* marco;
	bool libre;
	bool bitUso;
	bool bitModificado;
	char* timeStamp;
	int idPatota;
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
t_list* lista_segmentos_libres;
t_list* lista_tablas_segmentos;
t_list* tabla_asociadores_segmentos;
t_list* patotas_creadas;
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
pthread_mutex_t mutex_tocar_memoria;
pthread_mutex_t mutex_tablas_segmentos;

pthread_t hiloReceive;

//Server

void arrancar_servidor(void);
void serve_client(int*);
void procesar_mensaje_recibido(int, int);
t_tarea* obtener_tarea(t_tripulante* tripulante);
char* obtener_tareas(t_pcb* pcb);
t_tarea* obtener_tarea_archivo(char* tarea_string);
void obtener_listado_tripulantes(t_list*);

#endif /* MI_RAM_HQ_SRC_UTILS_RAM_H_ */
