#include "msg_RAM.h"

t_tripulante* deserializar_expulsar_tripulante(uint32_t socket_cliente) {
	//------------ORDEN------------
	//1. Id tripulante
	//2. Id Patota asociada
	//-----------------------------

	t_tripulante* tripulante = malloc(sizeof(t_tripulante));

	recv(socket_cliente, &(tripulante->id), sizeof(uint32_t), 0);
	recv(socket_cliente, &(tripulante->id_patota_asociado), sizeof(uint32_t), 0);

	return tripulante;
}

t_tripulante* deserializar_solicitar_tarea(uint32_t socket_cliente){
	//------------ORDEN------------
	//1. Id tripulante
	//2. Id Patota asociada
	//-----------------------------

	t_tripulante* tripulante = malloc(sizeof(t_tripulante));

	recv(socket_cliente, &(tripulante->id), sizeof(uint32_t), 0);
	recv(socket_cliente, &(tripulante->id_patota_asociado), sizeof(uint32_t), 0);

	return tripulante;
}


void enviar_solicitar_tarea_respuesta(t_tarea* tarea, uint32_t socket_cliente){

	t_paquete* paquete_a_enviar = crear_paquete(SOLICITAR_TAREA);
	serializar_solicitar_tarea_respuesta(tarea, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_cliente);
}

void serializar_solicitar_tarea_respuesta(t_tarea* msg, t_buffer* buffer){
	//------------ORDEN------------
	//1. Tarea
	//2. Parametro
	//3. Posicion X
	//4. Posicion Y
	//5. Tiempo
	//-----------------------------
	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t)*4 + sizeof(e_tarea);
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(msg->tarea), sizeof(e_tarea));
	offset += sizeof(e_tarea);

	memcpy(buffer->stream + offset, &(msg->parametro), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->posicion->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer->stream + offset, &(msg->posicion->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->tiempo), sizeof(uint32_t));
}





