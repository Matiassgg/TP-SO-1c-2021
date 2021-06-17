#include "msg_Mongo.h"

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

tarea_Mongo* desearilizar_ejecutar_tarea(uint32_t socket_cliente) {
	//------------ORDEN------------
	//1. ID
	//2. Tarea
	//-----------------------------

	tarea_Mongo* tarea = malloc(sizeof(tarea_Mongo));

	recv(socket_cliente, &(tarea->id), sizeof(uint32_t), 0);
	recv(socket_cliente, &(tarea->tamanio_tarea), sizeof(uint32_t), 0);
	recv(socket_cliente, tarea->tarea, tarea->tamanio_tarea, 0);

	return tarea;
}
