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

mover_hacia* deserializar_mover_hacia(uint32_t socket_cliente){
	//------------ORDEN------------
	//1. Id tripulante
	//2. Direccion
	//3. Id patota asociada
	//-----------------------------

	mover_hacia* mover_hacia = malloc(sizeof(mover_hacia));

	recv(socket_cliente, &(mover_hacia->id_tripulante), sizeof(uint32_t), 0);
	recv(socket_cliente, &(mover_hacia->direccion), sizeof(t_movimiento), 0);
	recv(socket_cliente, &(mover_hacia->id_patota_asociado), sizeof(uint32_t), 0);

	return mover_hacia;
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

	if(msg){
		buffer->size = sizeof(uint32_t)*5 + msg->tamanio_tarea;
		buffer->stream = malloc(buffer->size);

		memcpy(buffer->stream + offset, &(msg->tamanio_tarea), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, msg->tarea, msg->tamanio_tarea);
		offset += msg->tamanio_tarea;

		memcpy(buffer->stream + offset, &(msg->parametro), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, &(msg->posicion->pos_x), sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(buffer->stream + offset, &(msg->posicion->pos_y), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, &(msg->tiempo), sizeof(uint32_t));
	}
	else{
		buffer->size=0;
		buffer->stream=NULL;
	}
}

t_buffer* serializar_memoria_tareas(char* tareas){
	t_buffer* buffer = crear_buffer();

	buffer->size = string_length(tareas)+1;
	buffer->stream = string_duplicate(tareas);
	log_info(logger, "RAM :: Tareas obtenida:\n%s", buffer->stream);

	return buffer;
}

t_buffer* serializar_memoria_pcb(t_pcb* pcb){
	//------------ORDEN------------
	//1. PID
	//2. Tareas
	//-----------------------------
	t_buffer* buffer = crear_buffer();
	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t)*2;
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(pcb->pid), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer->stream + offset, &(pcb->tareas), sizeof(uint32_t));

	return buffer;
}

t_buffer* serializar_memoria_tcb(t_tcb* tcb){
	//------------ORDEN------------
	//1. TID
	//2. Estado
	//3. Posicion X
	//4. Posicion Y
	//5. Proxima tarea
	//6. Puntero PCB
	//-----------------------------
	t_buffer* buffer = crear_buffer();

	buffer->size = sizeof(uint32_t)*5+sizeof(char);
	buffer->stream = malloc(buffer->size);

	uint32_t offset = 0;

	memcpy(buffer->stream + offset, &(tcb->tid), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer->stream + offset, &(tcb->estado), sizeof(char));
	offset += sizeof(char);
	memcpy(buffer->stream + offset, &(tcb->posicion->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer->stream + offset, &(tcb->posicion->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer->stream + offset, &(tcb->prox_instruccion), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer->stream + offset, &(tcb->puntero_pcb), sizeof(uint32_t));

	return buffer;
}



