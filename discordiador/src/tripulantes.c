#include "tripulantes.h"

t_tripulante* obtener_tripulante_de_patota(t_patota* patota, int i){
	t_tripulante* tripulante = malloc(sizeof(t_tripulante));
	tripulante->posicion = malloc(sizeof(t_posicion));

	tripulante->id_patota_asociado = patota->id_patota;
	tripulante->posicion = list_get(patota->posiciones,i);
	tripulante->id = ++cantidad_tripulantes;
	log_info(logger, "El tripulante %i esta en la posicion %i,%i", tripulante->id, tripulante->posicion->pos_x,tripulante->posicion->pos_y);
	tripulante->tarea_act = NULL;
	tripulante->estado = NEW;

	return tripulante;
}


void solicitar_tarea(t_tripulante* tripulante){
	enviar_solicitar_tarea(tripulante, tripulante->socket_conexion_RAM);

	tripulante->tarea_act = recibir_tarea(tripulante->socket_conexion_RAM);
}

void ejecutar_tripulante(t_tripulante* tripulante){ // TODO ESTA MAL FALTA
	tripulante->socket_conexion_RAM = crear_conexion(ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ);

	enviar_iniciar_tripulante(tripulante, tripulante->socket_conexion_RAM);
//	TODO no hay que enviar el tripulante sino que ram debe crear todo con la patota ONO

	solicitar_tarea(tripulante);
	while(tripulante->tarea_act){
		p_tripulante* tripulante_plani = malloc(sizeof(p_tripulante));
		tripulante_plani->tripulante = tripulante;
		tripulante_plani->esta_activo = true;
		pthread_mutex_init(&tripulante_plani->mutex_ready, NULL);
		pthread_mutex_lock(&tripulante_plani->mutex_ready);

		hacer_tarea(tripulante);

		log_info(logger, "Se agrega tripulante a ready");
		pthread_mutex_lock(&mutex_cola_ready);
		queue_push(cola_ready, tripulante_plani);
//		p_tripulante* tripulante_plani2 = (p_tripulante*) queue_pop(cola_ready);
//		queue_push(cola_ready, tripulante_plani);
		pthread_mutex_unlock(&mutex_cola_ready);

		pthread_mutex_lock(&tripulante_plani->mutex_ready);

		while(quedan_pasos(tripulante) && puedo_seguir(tripulante_plani)){
			enviar_mover_hacia(tripulante, avanzar_hacia(tripulante, tripulante->tarea_act->posicion));
		}
		puedo_seguir(tripulante_plani);
		hacer_tarea(tripulante);
		tripulante_plani->esta_activo = false;

		solicitar_tarea(tripulante);
	}
//	TODO finalizar tripulante


	//TODO supongo que plani dira che vos movete y este wacho se mueve, asi?
}

bool puedo_seguir(p_tripulante* tripulante_plani){
	pthread_mutex_lock(&tripulante_plani->mutex_ready);

	return true;
}

void hacer_tarea(t_tripulante* tripulante){
	rafaga_cpu();

	switch(tripulante->tarea_act->tarea){
		case GENERAR_OXIGENO:
			log_info(logger, "GENERAR_OXIGENO");
			;
		break;
		case CONSUMIR_OXIGENO:
			log_info(logger, "CONSUMIR_OXIGENO");
			;
		break;
		case GENERAR_COMIDA:
			log_info(logger, "GENERAR_COMIDA");
			;
		break;
		case CONSUMIR_COMIDA:
			log_info(logger, "CONSUMIR_COMIDA");
			;
		break;
		case GENERAR_BASURA:
			log_info(logger, "GENERAR_BASURA");
			;
		break;
		case DESCARTAR_BASURA:
			log_info(logger, "DESCARTAR_BASURA");
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

t_movimiento avanzar_hacia(t_tripulante* tripulante, t_posicion* posicion_meta) {
	t_movimiento direccion;
	if (quedan_movimientos(tripulante->posicion->pos_x, posicion_meta->pos_x)) {
		if (tripulante->posicion->pos_x < posicion_meta->pos_x) {
			tripulante->posicion->pos_x++;
			direccion = DERECHA;
		} else {
			tripulante->posicion->pos_x--;
			direccion = IZQUIERDA;
		}
	} else {
		if (quedan_movimientos(tripulante->posicion->pos_y, posicion_meta->pos_y)) {
			if (tripulante->posicion->pos_y < posicion_meta->pos_y) {
				tripulante->posicion->pos_y++;
				direccion = ABAJO;
			} else {
				tripulante->posicion->pos_y--;
				direccion = ARRIBA;
			}
		}
	}
	rafaga_cpu();
	log_info(logger, "El tripulante %i esta ahora en la posicion %i,%i yendo a %i,%i", tripulante->id, tripulante->posicion->pos_x,tripulante->posicion->pos_y, posicion_meta->pos_x, posicion_meta->pos_y);
	return direccion;
}
