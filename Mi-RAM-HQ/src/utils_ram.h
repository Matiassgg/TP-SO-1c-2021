#ifndef MI_RAM_HQ_SRC_UTILS_RAM_H_
#define MI_RAM_HQ_SRC_UTILS_RAM_H_

#include "../../utils/utils/utils.h"
#include "msg_RAM.h"
#include "mapa.h"
#include "signal.h"
#include "unistd.h"

#define TAM_GUIONES 74

//---------FUNCIONES--------//
void* seleccionar_victima;
//-------------------------//

//---------CONFIG--------//
char* ip_Mi_RAM_HQ;
char* puerto_escucha;

char* ip_discordiador;
char* puerto_discordiador;
uint32_t socket_discordiador;

char* archivo_log;

uint32_t tamanio_memoria;
char* esquema_memoria;

uint32_t tamanio_pagina;
uint32_t tamanio_swap;
char* path_swap;

char* algoritmo_reemplazo;
char* esquema_memoria;
char* criterio_seleccion;
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
	t_dictionary* diccionario_segmentos;
	uint32_t id_patota_asociada;
	t_list* tripulantes_activos;
} t_tabla_segmentos;

//typedef struct {
//	uint32_t nro_segmento;
//	uint32_t id_asociado;
//	e_tipo_dato tipo_dato;
//} t_asociador_segmento;

//---------Paginación simple--------//

typedef struct {
	uint32_t inicioMemoria;
	uint32_t numeroMarco;
	bool bitUso;
	char* timeStamp;
	int idPatota;
} t_marco;

typedef struct {
	uint32_t numeroPagina;
	t_marco* marco;
	t_dictionary* diccionario_pagina;
	bool bit_presencia;
	bool bit_modificado;
	uint32_t espacio_libre;
} t_pagina;

typedef struct {
	uint32_t inicio;
	uint32_t tamanio;
} t_asociador_pagina;

typedef struct {
	t_list* paginas;
	uint32_t id_patota_asociada;
	t_list* tripulantes_activos;
} t_tabla_paginas;

typedef struct {
	uint32_t inicioSwap;
	t_marco* marco;
	t_pagina* pagina;
	int numeroMarcoSwap;
	uint32_t bitUso;
	int idPatota;
} t_marco_en_swap;

//---------MAIN--------//
t_config* config; //1
t_log* logger; //1
uint32_t socketConexionApp;//1
void* memoria;
void* memoria_virtual;
FILE* espacio_swap;
uint32_t tamanio_pagina;
t_list* lista_segmentos_libres;
t_list* lista_tablas_segmentos;
t_list* tabla_asociadores_segmentos;
t_list* lista_tablas_paginas;
t_list* patotas_creadas;
t_marco* punteroMarcoClock;
t_list* tablaDeMarcos;
t_list* marcos_swap;
t_list* tareas;
char* path_tareas;
char* path_dump;
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
pthread_mutex_t mutex_subir_patota;
pthread_mutex_t mutex_tablas;
pthread_mutex_t mutex_marcos;

pthread_t hiloReceive;

//Server

void arrancar_servidor(void);
void serve_client(int*);
void procesar_mensaje_recibido(int, int);
t_tarea* obtener_tarea(t_tripulante* tripulante);
char* obtener_tareas(t_patota* patota);
t_tarea* obtener_tarea_archivo(char* tarea_string);
t_list* obtener_listado_tripulantes(void);

#endif /* MI_RAM_HQ_SRC_UTILS_RAM_H_ */
