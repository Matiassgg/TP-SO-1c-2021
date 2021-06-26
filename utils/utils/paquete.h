#ifndef SHARED_SHARED_PAQUETE_H_
#define SHARED_SHARED_PAQUETE_H_

#include "sockets.h"
#include "buffer.h"
#include <commons/collections/list.h>
#include <commons/string.h>

//typedef enum
//{
//	MENSAJE,
//	PAQUETE
//}op_code; // ?????

//---------MENSAJES--------//
typedef enum {
	MSGTYPE_UNKNOWN = 0,
	// Discordiador
	INICIAR_PLANIFICACION,
	PAUSAR_PLANIFICACION,
	INICIAR_PATOTA,
	INICIAR_TRIPULANTE,
	LISTAR_TRIPULANTES,
	EXPULSAR_TRIPULANTE,
	OBTENER_BITACORA,
	ESTA_ON,
	SABOTAJE,
	// Tripulantes ?
	ARRANCAR_TAREAS,					// 1.	Informar al módulo Mi-RAM HQ que desea iniciar, indicando a qué patota pertenece
	MOVER_HACIA,
	SOLICITAR_TAREA,
	COMENZAR_EJECUCION_TAREA,
	FINALIZAR_TAREA,
//	GENERAR_OXIGENO,
//	CONSUMIR_OXIGENO,
//	GENERAR_COMIDA,
//	CONSUMIR_COMIDA,
//	GENERAR_BASURA,
//	DESCARTAR_BASURA,
	// Respuesta
	RESPUESTA
}op_code;


typedef struct {
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

t_paquete* crear_paquete(op_code codigo_operacion);
void agregar_a_paquete(t_paquete* paquete, char* valor);
t_buffer* serializar_paquete(t_paquete* paquete);
void eliminar_paquete(t_paquete* paquete);
void enviar_paquete(t_paquete* paquete, int socket_cliente);

#endif /* SHARED_SHARED_PAQUETE_H_ */
