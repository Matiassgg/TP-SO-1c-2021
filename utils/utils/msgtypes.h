#ifndef SHARED_SHARED_MSGTYPES_H_
#define SHARED_SHARED_MSGTYPES_H_

#include "paquete.h"

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
} t_iniciar_patota;

//---------ESTADO--------//
typedef enum {
	NEW,
	READY,
	BLOCKED,
	EXEC,
	EXIT
} t_estado;

t_iniciar_patota* deserializar_iniciar_patota(uint32_t socket_cliente);
t_respuesta recibir_respuesta(uint32_t socket_conexion);
void enviar_respuesta(t_respuesta respuesta, uint32_t socket_conexion);
void serializar_respuesta(t_respuesta respuesta, t_buffer* buffer);

#endif /* SHARED_SHARED_MSGTYPES_H_ */
