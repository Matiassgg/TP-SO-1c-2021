#ifndef SHARED_SHARED_MSGTYPES_H_
#define SHARED_SHARED_MSGTYPES_H_

#include "protocolo.h"

#define MSGTYPES_QTY 1+12	// Check ???

typedef enum
{
	OPCODE_UNKNOWN = 0
	// ???
}e_opcode; //????????

typedef enum {
	MODULO_DESCONOCIDO = 0,
	MODULO_DISCORDIADOR = 1,
	MODULO_TRIPULANTE = 1,	// ???
	MODULO_MI_RAM_HQ = 2,
	MODULO_I_MONGO_STORE = 3,
}e_module;


typedef enum {
	OK,
	FAIL
} t_respuesta;

typedef struct {
	uint32_t pos_x;
	uint32_t pos_y;
} t_posicion;

//---------TRIPULANTE--------//
typedef struct {
	uint32_t id_tripulante;
	uint32_t id_patota;
	uint32_t status;
	t_posicion posicion;
	t_list* tareas_act;		//
} t_tripulante;


//---------PATOTA--------//
typedef struct {
	uint32_t id_patota;
    uint32_t cant_tripulantes;
    uint32_t tam_path;
    char* path_tareas;
    t_list* posiciones;
} t_iniciar_patota;

t_iniciar_patota* deserializar_iniciar_patota(uint32_t socket_cliente);
t_respuesta recibir_respuesta(uint32_t socket_conexion);
void enviar_respuesta(t_respuesta respuesta, uint32_t socket_conexion);
void serializar_respuesta(t_respuesta respuesta, t_buffer* buffer);

#endif /* SHARED_SHARED_MSGTYPES_H_ */
