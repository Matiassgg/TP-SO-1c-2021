#include "msgtypes.h"

iniciar_patota* deserializar_iniciar_patota(uint32_t socket_cliente){
	//------------ORDEN------------
	//1. Cantidad tripulantes
	//2. Tamaño del path
	//3. Path tareas
	//4. Posiciones tripulantes 	} COMO ES LISTA IRA DENTRO DE UN FOR
	//-----------------------------
	iniciar_patota* msg = malloc(sizeof(iniciar_patota));
	msg->posiciones = list_create();

	recv(socket_cliente, &(msg->cant_tripulantes), sizeof(uint32_t), 0);

	recv(socket_cliente, &(msg->tam_path), sizeof(uint32_t), 0);
	msg->path_tareas = malloc(msg->tam_path);

	recv(socket_cliente, msg->path_tareas, msg->tam_path, 0);
	for (int i = 0; i < (msg->cant_tripulantes); ++i) {
		posicion posiciones;

		recv(socket_cliente, &(posiciones->pos_x), sizeof(uint32_t), 0);
		recv(socket_cliente, &(posiciones->pos_y), sizeof(uint32_t), 0);

		list_add(msg->posiciones, posiciones);
	}

	return msg;
}
