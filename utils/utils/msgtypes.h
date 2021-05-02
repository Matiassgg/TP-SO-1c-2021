#ifndef SHARED_SHARED_MSGTYPES_H_
#define SHARED_SHARED_MSGTYPES_H_

#include "paquete.h"

typedef enum {
	OK,
	FAIL
} t_respuesta;

//---------ESTADO--------//
typedef enum {
	NEW,
	READY,
	BLOCKED_I_O,
	BLOCKED_EMERG,
	EXEC,
	EXIT
} t_estado;

typedef struct {
	uint32_t pos_x;
	uint32_t pos_y;
} t_posicion;

//typedef struct{
//	uint32_t id_pcb_patota;
//    uint32_t tam_path;
//    void* path_tareas;
//} t_pcb_patota;
//
//typedef struct{
//	uint32_t id_tcb_tripulante;
//    t_estado estado;
//    t_posicion posicion;
//    uint32_t id_prox_instruc;
//    uint32_t tam_path;
//    char* path_tareas;
//} t_tcb_tripulante;
//????????

typedef struct{
	uint32_t pid;
	void* tareas;
} t_pcb;

typedef struct{
	uint32_t tid;
	t_estado estado;
	t_posicion posicion;
	uint32_t prox_instruccion;
} t_tcb;

//---------TRIPULANTE--------//
typedef struct {
	uint32_t id_tripulante;
	uint32_t id_patota;
	t_posicion posicion;
	t_list* tareas_act;
} t_tripulante;

typedef struct {
	uint32_t id_tripulante;
	uint32_t id_patota;
	uint32_t status;
} t_listar_tripulantes;

//---------PATOTA--------//
typedef struct {
	uint32_t id_patota;
    uint32_t cant_tripulantes;
    uint32_t tam_path;
    char* path_tareas;
    t_list* posiciones;
} t_patota;

t_patota* deserializar_iniciar_patota(uint32_t socket_cliente);
t_respuesta recibir_respuesta(uint32_t socket_conexion);
void enviar_respuesta(t_respuesta respuesta, uint32_t socket_conexion);
void serializar_respuesta(t_respuesta respuesta, t_buffer* buffer);

#endif /* SHARED_SHARED_MSGTYPES_H_ */
