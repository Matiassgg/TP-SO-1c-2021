#include "msg_Mongo.h"

void enviar_discordiador_sabotaje(t_posicion* posicion_sabotaje, uint32_t socket_discordiador) {
	t_paquete* paquete_a_enviar = crear_paquete(SABOTAJE);
	serializar_sabotaje_en_mongo(posicion_sabotaje, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_discordiador);
}

t_mover_hacia* deserializar_mover_hacia_posicion(uint32_t socket_cliente){
	//------------ORDEN------------
	//1. ID
	//2. Posicion origen
	//3. Posicion destino
	//-----------------------------

	t_mover_hacia* posicion = malloc(sizeof(t_mover_hacia));

	recv(socket_cliente, &(posicion->id_tripulante), sizeof(uint32_t), 0);
	recv(socket_cliente, &(posicion->posicion_origen->pos_x), sizeof(uint32_t), 0);
	recv(socket_cliente, &(posicion->posicion_origen->pos_y), sizeof(uint32_t), 0);
	recv(socket_cliente, &(posicion->posicion_destino->pos_x), sizeof(uint32_t), 0);
	recv(socket_cliente, &(posicion->posicion_destino->pos_y), sizeof(uint32_t), 0);

	return posicion;
}

tarea_Mongo* deserializar_tarea(uint32_t socket_cliente) {
	//------------ORDEN------------
	//1. ID
	//2. Tamanio
	//3. Tarea
	//4. Parametro
	//-----------------------------

	tarea_Mongo* tarea = malloc(sizeof(tarea_Mongo));

	recv(socket_cliente, &(tarea->id), sizeof(uint32_t), 0);
	recv(socket_cliente, &(tarea->tamanio_tarea), sizeof(uint32_t), 0);
	tarea->tarea = calloc(1,tarea->tamanio_tarea);
	recv(socket_cliente, tarea->tarea, tarea->tamanio_tarea, 0);
	recv(socket_cliente, &(tarea->parametro), sizeof(uint32_t), 0);

	return tarea;
}

void serializar_sabotaje_en_mongo(t_posicion* posicion, t_buffer* buffer) {
	//------------ORDEN------------
	//1. Posicion x
	//2. Posicion y
	//-----------------------------
	uint32_t offset = 0;
	buffer->size = sizeof(t_posicion);
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(posicion->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(posicion->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);
}

