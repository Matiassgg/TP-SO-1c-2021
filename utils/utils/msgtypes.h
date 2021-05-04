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

typedef struct{
	uint32_t pid; //Identificador de la Patota
	uint32_t tareas; //Dirección lógica del inicio de las Tareas
} t_pcb;

typedef struct{
	uint32_t tid; // Identificador del tripulante
	t_estado estado; // Estado del tripulante (N/R/E/B)
	t_posicion* posicion; // Posición del tripulante
	uint32_t prox_instruccion; // Dirección lógica del PCB del tripulante
	uint32_t puntero_pcb; // Identificador de la próxima instrucción a ejecutar
} t_tcb;

//---------TAREAS--------//
typedef enum {
	GENERAR_OXIGENO,
	CONSUMIR_OXIGENO,
	GENERAR_COMIDA,
	CONSUMIR_COMIDA,
	GENERAR_BASURA,
	DESCARTAR_BASURA
} e_tarea;

typedef struct {
	e_tarea tarea;
    uint32_t parametro;
	t_posicion* posicion;
    uint32_t tiempo;
} t_tarea;

//---------TRIPULANTE--------//
typedef struct {
	uint32_t id;
	uint32_t id_patota_asociado;
	t_posicion* posicion;
//	t_list* tarea_act; // DEBERIA SER UNA LISTA?? SOLO HACE DE A UNA TAREA
	t_tarea* tarea_act;
	t_estado estado;
	bool quantum;
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



t_patota* deserializar_iniciar_patota(uint32_t);
t_tripulante* deserializar_iniciar_tripulante(uint32_t socket_cliente);
t_respuesta recibir_respuesta(uint32_t);
void enviar_respuesta(t_respuesta, uint32_t);
void serializar_respuesta(t_respuesta, t_buffer*);

#endif /* SHARED_SHARED_MSGTYPES_H_ */
