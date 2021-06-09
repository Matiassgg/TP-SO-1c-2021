#include "msg_discordiador.h"

void enviar_iniciar_patota(t_patota* msg, uint32_t socket_conexion){
	t_paquete* paquete_a_enviar = crear_paquete(INICIAR_PATOTA);
	serializar_iniciar_patota(msg, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}
void enviar_iniciar_tripulante(t_tripulante* tripulante, uint32_t socket_conexion){
	t_paquete* paquete_a_enviar = crear_paquete(INICIAR_TRIPULANTE);
	serializar_iniciar_tripulante(tripulante, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_solicitar_tarea(t_tripulante* tripulante, uint32_t socket_conexion){
	t_paquete* paquete_a_enviar = crear_paquete(SOLICITAR_TAREA);
	serializar_solicitar_tarea(tripulante, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_RAM_listar_tripulantes(t_tripulante* tripulante, uint32_t socket_conexion){
	t_paquete* paquete_a_enviar = crear_paquete(LISTAR_TRIPULANTES);
	serializar_listar_tripulantes(tripulante, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_RAM_expulsar_tripulante(t_tripulante* msg, uint32_t socket_conexion) {
	t_paquete* paquete_a_enviar = crear_paquete(EXPULSAR_TRIPULANTE);
	serializar_expulsar_tripulante(msg, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_Mongo_ejecutar_tarea(t_tripulante* msg, uint32_t socket_conexion) {
	t_paquete* paquete_a_enviar = crear_paquete(COMENZAR_EJECUCION_TAREA);
	serializar_ejecutar_tarea(msg, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_mover_hacia(t_tripulante* tripulante, t_movimiento direccion){
//	RAM
	t_paquete* paquete_a_enviar = crear_paquete(MOVER_HACIA);
	serializar_mover_hacia_RAM(tripulante->id, direccion, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, tripulante->socket_conexion_RAM);
//	MONGO
	paquete_a_enviar = crear_paquete(MOVER_HACIA);
	serializar_mover_hacia_Mongo(tripulante, direccion, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, tripulante->socket_conexion_Mongo);

}
void serializar_iniciar_patota(t_patota* msg, t_buffer* buffer){
	//------------ORDEN------------
	//1. Cantidad tripulantes
	//2. Tamaño del path
	//3. Path tareas
	//4. Posiciones tripulantes 	} COMO ES LISTA IRA DENTRO DE UN FOR
	//5. Id patota
	//-----------------------------

	uint32_t offset = 0;
	msg->tam_path = string_length(msg->path_tareas)+1;

	buffer->size = sizeof(uint32_t)*3;
	buffer->size += msg->tam_path;
	buffer->size += (2 * sizeof(uint32_t)) * msg->cant_tripulantes;

	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(msg->cant_tripulantes), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->tam_path), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, msg->path_tareas, msg->tam_path);
	offset += msg->tam_path;

	for (int i = 0; i < msg->cant_tripulantes; i++) {
		t_posicion* posicion = list_get(msg->posiciones,i);

		memcpy(buffer->stream + offset, &(posicion->pos_x), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, &(posicion->pos_y), sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

	memcpy(buffer->stream + offset, &(msg->id_patota), sizeof(uint32_t));
}

void serializar_iniciar_tripulante(t_tripulante* msg, t_buffer* buffer){
	//------------ORDEN------------
	//1. ID
	//2. Patota asociada
	//3. Posicion X
	//4. Posicion Y
	//-----------------------------
	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t)*4;
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(msg->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->id_patota_asociado), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->posicion->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer->stream + offset, &(msg->posicion->pos_y), sizeof(uint32_t));
}

void serializar_ejecutar_tarea(uint32_t id_tripulante, e_tarea tarea, t_buffer* buffer) {
	//------------ORDEN------------
	//1. ID
	//2. Tarea
	//-----------------------------
	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t) + sizeof(e_tarea);
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(id_tripulante), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(tarea), sizeof(e_tarea));
}

void serializar_finalizar_tarea(uint32_t id_tripulante, e_tarea tarea, t_buffer* buffer) {
	//------------ORDEN------------
	//1. ID
	//2. Tarea
	//-----------------------------
	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t) + sizeof(e_tarea);
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(id_tripulante), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(tarea), sizeof(e_tarea));
}

void serializar_mover_hacia_RAM(uint32_t id_tripulante, t_movimiento direccion, t_buffer* buffer){
	//------------ORDEN------------
	//1. ID
	//2. Direccion
	//-----------------------------
	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t) + sizeof(t_movimiento);
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(id_tripulante), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(direccion), sizeof(t_movimiento));
}

void serializar_mover_hacia_Mongo(t_tripulante* tripulante,t_movimiento direccion, t_buffer* buffer){
	//------------ORDEN------------
	//1. ID
	//2. Posicion origen
	//3. Posicion destino
	//-----------------------------
	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t)*5;
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(tripulante->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	t_posicion posicion_origen = obtener_posicion_origen(tripulante->posicion, direccion);
	memcpy(buffer->stream + offset, &(posicion_origen.pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(posicion_origen.pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(tripulante->posicion->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(tripulante->posicion->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);
}

void serializar_listar_tripulantes(t_tripulante* msg, t_buffer* buffer) {
	//------------ORDEN------------
	//1. Id del tripulante
	//2. Id de patota
	//3. Estado
	//-----------------------------

	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t)*3;
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(msg->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->id_patota_asociado), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->estado), sizeof(uint32_t));
	offset += sizeof(uint32_t);
}

void serializar_expulsar_tripulante(t_tripulante* msg, t_buffer* buffer) {
	//------------ORDEN------------
	//1. Id tripulante
	//2. Id Patota asociada
	//-----------------------------
	// Sera necesaria la posicion ? Si se la informo a RAM siempre ...

	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t)*2 + sizeof(t_posicion);
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(msg->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->id_patota_asociado), sizeof(uint32_t));
	offset += sizeof(uint32_t);

}

void serializar_solicitar_tarea(t_tripulante* msg, t_buffer* buffer){
	//------------ORDEN------------
	//1. Id tripulante
	//2. Id Patota asociada
	//-----------------------------

	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t)*2 + sizeof(t_posicion);
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(msg->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->id_patota_asociado), sizeof(uint32_t));
	offset += sizeof(uint32_t);

}

t_tarea* recibir_tarea(uint32_t socket_cliente){
	int aux;
	recv(socket_cliente, &aux, sizeof(op_code), MSG_WAITALL);
	recv(socket_cliente, &aux, sizeof(uint32_t), MSG_WAITALL);

	return deserializar_solicitar_tarea_respuesta(socket_cliente);

}

t_tarea* deserializar_solicitar_tarea_respuesta(uint32_t socket_cliente) {
	//------------ORDEN------------
	//1. Tarea
	//2. Parametro
	//3. Posicion X
	//4. Posicion Y
	//5. Tiempo
	//-----------------------------

	t_tarea* tarea = malloc(sizeof(t_tarea));
	tarea->posicion = malloc(sizeof(t_posicion));

	recv(socket_cliente, &(tarea->tarea), sizeof(e_tarea), 0);
	recv(socket_cliente, &(tarea->parametro), sizeof(uint32_t), 0);
	recv(socket_cliente, &(tarea->posicion->pos_x), sizeof(uint32_t), 0);
	recv(socket_cliente, &(tarea->posicion->pos_y), sizeof(uint32_t), 0);
	recv(socket_cliente, &(tarea->tiempo), sizeof(uint32_t), 0);

	return tarea;
}

