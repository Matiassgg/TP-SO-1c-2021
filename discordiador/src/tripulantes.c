#include "tripulantes.h"

t_tripulante* obtener_tripulante_de_patota(t_patota* patota, int i){
	t_tripulante* tripulante = malloc(sizeof(t_tripulante));

	tripulante->id_patota_asociado = patota->id_patota;
	tripulante->posicion = list_get(patota->posiciones,i);
	tripulante->id = i;
	tripulante->tarea_act = NULL;
	tripulante->estado = NEW;

	return tripulante;
}

void iniciar_tripulante(t_tripulante* tripulante){
	int socket_RAM = crear_conexion(ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ);

	enviar_iniciar_tripulante(tripulante, socket_RAM);

//	solicitar_tarea(tripulante);
	queue_push(cola_ready, tripulante);
}
