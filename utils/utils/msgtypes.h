#ifndef SHARED_SHARED_MSGTYPES_H_
#define SHARED_SHARED_MSGTYPES_H_

#include "protocolo.h"

typedef enum {
	MSGTYPE_UNKNOWN = 0,
	// Discordiador
	INICIAR_PLANIFICACION,
	PAUSAR_PLANIFICACION,
	INICIAR_PATOTA,
	LISTAR_TRIPULANTES,
	EXPULSAR_TRIPULANTE,
	OBTENER_BITACORA,
	ESTA_ON,
	// Tripulantes ?
	GENERAR_OXIGENO,
	CONSUMIR_OXIGENO,
	GENERAR_COMIDA,
	CONSUMIR_COMIDA,
	GENERAR_BASURA,
	DESCARTAR_BASURA,
}e_msgtype;

#define MSGTYPES_QTY 1+12	// Check

typedef enum
{
	OPCODE_UNKNOWN = 0
	// ???
}e_opcode;

typedef enum {

	MODULO_DESCONOCIDO = 0,
	MODULO_DISCORDIADOR = 1,
	MODULO_TRIPULANTE = 1,	// ???
	MODULO_MI_RAM_HQ = 2,
	MODULO_I_MONGO_STORE = 3,

}e_module;

typedef struct{
    uint32_t cant_tripulantes;
    uint32_t tam_path;
    char* path_tareas;
    t_list* posiciones;
} iniciar_patota;

typedef struct{
	uint32_t pos_x;
	uint32_t pos_y;
} posicion;

#endif /* SHARED_SHARED_MSGTYPES_H_ */
