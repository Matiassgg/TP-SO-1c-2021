#include "msg_discordiador.h"

void enviar_iniciar_patota(t_iniciar_patota* msg, uint32_t socket_conexion){
	t_paquete* paquete_a_enviar = crear_paquete(INICIAR_PATOTA);

	serializar_iniciar_patota(msg, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void serializar_iniciar_patota(t_iniciar_patota* msg, t_buffer* buffer){
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
