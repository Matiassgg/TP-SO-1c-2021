#include "msgtypes.h"

t_patota* deserializar_iniciar_patota(uint32_t socket_cliente){
	//------------ORDEN------------
	//1. Cantidad tripulantes
	//2. Tamaño del path
	//3. Path tareas
	//4. Posiciones tripulantes 	} COMO ES LISTA IRA DENTRO DE UN FOR
	//5. Id patota
	//-----------------------------
	t_patota* msg = malloc(sizeof(t_patota));
	msg->posiciones = list_create();

	recv(socket_cliente, &(msg->cant_tripulantes), sizeof(uint32_t), 0);

	recv(socket_cliente, &(msg->tam_path), sizeof(uint32_t), 0);
	msg->path_tareas = malloc(msg->tam_path);
	recv(socket_cliente, msg->path_tareas, msg->tam_path, 0);

	for (int i = 0; i < (msg->cant_tripulantes); i++) {
		t_posicion* posiciones = malloc(sizeof(t_posicion));
		recv(socket_cliente, &(posiciones->pos_x), sizeof(uint32_t), 0);
		recv(socket_cliente, &(posiciones->pos_y), sizeof(uint32_t), 0);
		list_add(msg->posiciones, posiciones);
	}
	recv(socket_cliente, &(msg->id_patota), sizeof(uint32_t), 0);

	return msg;
}

t_tripulante* deserializar_iniciar_tripulante(uint32_t socket_cliente){
	//------------ORDEN------------
	//1. ID
	//2. Patota asociada
	//3. Posicion X
	//4. Posicion Y
	//-----------------------------
	t_tripulante* msg = malloc(sizeof(t_tripulante));
	msg->posicion = malloc(sizeof(t_posicion));

	recv(socket_cliente, &(msg->id), sizeof(uint32_t), 0);

	recv(socket_cliente, &(msg->id_patota_asociado), sizeof(uint32_t), 0);

	recv(socket_cliente, &(msg->posicion->pos_x), sizeof(uint32_t), 0);
	recv(socket_cliente, &(msg->posicion->pos_y), sizeof(uint32_t), 0);

	return msg;
}

void enviar_respuesta(t_respuesta respuesta, uint32_t socket_conexion){
	t_paquete* paquete_a_enviar = crear_paquete(RESPUESTA);

	serializar_respuesta(respuesta, paquete_a_enviar->buffer);

	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void serializar_respuesta(t_respuesta respuesta, t_buffer* buffer){
	//1. Respuesta

	buffer->size = sizeof(t_respuesta);

	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream, &(respuesta), sizeof(t_respuesta));
}

t_respuesta recibir_respuesta(uint32_t socket_conexion){
	t_respuesta respuesta = FAIL;
	uint32_t aux;

	recv(socket_conexion, &aux, sizeof(op_code), MSG_WAITALL);
	recv(socket_conexion, &aux, sizeof(uint32_t), MSG_WAITALL);
	recv(socket_conexion, &respuesta, sizeof(t_respuesta), 0);

	return respuesta;
}





