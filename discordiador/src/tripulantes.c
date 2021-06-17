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
		log_info(logger, "Se agrega tripulante a ready");

		pthread_mutex_lock(&mutex_cola_ready);
		queue_push(cola_ready, tripulante_plani);
		pthread_mutex_unlock(&mutex_cola_ready);

		pthread_mutex_lock(&tripulante_plani->mutex_ready);

		while(quedan_pasos(tripulante) && puedo_seguir(tripulante_plani)){
			enviar_mover_hacia(tripulante, avanzar_hacia(tripulante, tripulante->tarea_act->posicion));
		}
		puedo_seguir(tripulante_plani);
		hacer_tarea(tripulante_plani);
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

void hacer_tarea(p_tripulante* tripulante_plani){
	rafaga_cpu(1);

	if(es_tarea_IO(tripulante_plani->tripulante->tarea_act->tarea)){
//		pthread_mutex_lock(&mutex_cola_bloqueados_io);
//		queue_push(cola_bloq_E_S,tripulante_plani);
		tripulante_plani->esta_activo = false;
//		pthread_mutex_unlock(&mutex_cola_bloqueados_io);
	}

	hacer_ciclos_tarea(tripulante_plani->tripulante);

}

bool es_tarea_IO(char* tarea){
	return (
			son_iguales(tarea, "GENERAR_OXIGENO") ||
			son_iguales(tarea, "CONSUMIR_OXIGENO") ||
			son_iguales(tarea, "GENERAR_COMIDA") ||
			son_iguales(tarea, "CONSUMIR_COMIDA") ||
			son_iguales(tarea, "GENERAR_BASURA") ||
			son_iguales(tarea, "DESCARTAR_BASURA")
			);
}

void hacer_ciclos_tarea(t_tripulante* tripulante){
	if(son_iguales(algoritmo,"FIFO")){
		log_info(logger, "El tripulante %i comenzara la tarea %s", tripulante->id, tripulante->tarea_act->tarea);
		rafaga_cpu(tripulante->tarea_act->tiempo);
		log_info(logger, "El tripulante %i finalizo la tarea %s", tripulante->id, tripulante->tarea_act->tarea);
	}
	else if(tripulante->tarea_act->tiempo > quantum){
		log_info(logger, "El tripulante %i comenzara la tarea %s", tripulante->id, tripulante->tarea_act->tarea);
		rafaga_cpu(quantum);
		log_info(logger, "El tripulante %i finalizo la tarea %s", tripulante->id, tripulante->tarea_act->tarea);
		tripulante->tarea_act->tiempo -= quantum;

		// TODO Y AHORA KE creo que habria que volver a ponerlo en ready, enotnces las colas de bloqueados de que va a servir? (estructura)
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
	rafaga_cpu(1);
	log_info(logger, "El tripulante %i esta ahora en la posicion %i,%i yendo a %i,%i", tripulante->id, tripulante->posicion->pos_x,tripulante->posicion->pos_y, posicion_meta->pos_x, posicion_meta->pos_y);
	return direccion;
}
