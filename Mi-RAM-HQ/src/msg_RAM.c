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

