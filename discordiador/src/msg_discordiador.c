#include "msg_discordiador.h"

void enviar_iniciar_patota(t_iniciar_patota* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(INICIAR_PATOTA);

	serializar_iniciar_patota(msg, paquete_a_enviar->buffer);
	t_buffer* buffer = serializar_paquete(paquete_a_enviar);

	send(socketConexion, buffer->stream, (size_t) buffer->size, 0);

	eliminar_paquete(paquete_a_enviar);
	eliminar_buffer(buffer);
}

void serializar_iniciar_patota(t_iniciar_patota* msg, t_buffer* buffer){
	//------------ORDEN------------
	//1. Cantidad tripulantes
	//2. Tamaño del path
	//3. Path tareas
	//4. Posiciones tripulantes 	} COMO ES LISTA IRA DENTRO DE UN FOR
	//-----------------------------

	uint32_t offset = 0;
	msg->tam_path = string_length(msg->path_tareas);

	buffer->size = sizeof(uint32_t);
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
		uint32_t posicion_x = (uint32_t) list_get(msg->posiciones,i);
		uint32_t posicion_y = (uint32_t) list_get(msg->posiciones,i);

		memcpy(buffer->stream + offset, &(posicion_x), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, &(posicion_y), sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

}
