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
	tripulante->socket_conexion = crear_conexion(ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ);

	enviar_iniciar_tripulante(tripulante, tripulante->socket_conexion);

//	solicitar_tarea(tripulante);
	queue_push(cola_ready, tripulante);
}

void hacer_tarea(t_tripulante* tripulante){
	switch(tripulante->tarea_act->tarea){
		case GENERAR_OXIGENO:
			;
		break;
		case CONSUMIR_OXIGENO:
			;
		break;
		case GENERAR_COMIDA:
			;
		break;
		case CONSUMIR_COMIDA:
			;
		break;
		case GENERAR_BASURA:
			;
		break;
		case DESCARTAR_BASURA:
			;
		break;
	}
}

bool quedan_pasos(t_tripulante* tripulante){
	return !esta_en_el_lugar(tripulante->posicion, tripulante->tarea_act->posicion);
}

bool esta_en_el_lugar(t_posicion* posicion1, t_posicion* posicion2){
	return (posicion1->pos_x == posicion2->pos_x) && (posicion1->pos_y == posicion2->pos_y);
}

bool quedan_movimientos(uint32_t posicion1, uint32_t posicion2) {
	return posicion1 != posicion2;
}

void avanzar_hacia(t_tripulante* tripulante, t_posicion posicion_meta) {
	if (quedan_movimientos(tripulante->posicion->pos_x, posicion_meta.pos_x)) {
		if (tripulante->posicion->pos_x < posicion_meta.pos_x) {
			tripulante->posicion->pos_x++;
		} else {
			tripulante->posicion->pos_x--;
		}
	} else {

		if (quedan_movimientos(tripulante->posicion->pos_y, posicion_meta.pos_y)) {
			if (tripulante->posicion->pos_y < posicion_meta.pos_y) {
				tripulante->posicion->pos_y++;
			} else {
				tripulante->posicion->pos_y--;
			}
		}
	}
	log_info(logger, "El tripualnte %i esta en la posicion %i,%i yendo a %i,%i", tripulante->id, tripulante->posicion->pos_x,tripulante->posicion->pos_y, posicion_meta.pos_x, posicion_meta.pos_y);
}
