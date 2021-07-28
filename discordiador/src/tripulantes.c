#include "tripulantes.h"

void subir_tripulante_ready(p_tripulante* tripulante_plani){
	log_info(logger, "Se agrega al tripulante %i a READY", tripulante_plani->tripulante->id);
	tripulante_plani->tripulante->estado = READY;
	enviar_RAM_actualizar_estado(tripulante_plani->tripulante,tripulante_plani->tripulante->socket_conexion_RAM);
	pthread_mutex_lock(&mutex_cola_ready);
	queue_push(cola_ready, tripulante_plani);
	pthread_mutex_unlock(&mutex_cola_ready);
	sem_post(&semaforo_cola_ready);
}

void subir_tripulante_bloqueado(p_tripulante* tripulante_plani){
	log_info(logger, "Se agrega al tripulante %i a BLOCKED_I_O", tripulante_plani->tripulante->id);
	tripulante_plani->tripulante->estado = BLOCKED_I_O;
	enviar_RAM_actualizar_estado(tripulante_plani->tripulante,tripulante_plani->tripulante->socket_conexion_RAM);
	pthread_mutex_lock(&mutex_cola_bloqueados_io);
	queue_push(cola_bloq_E_S, tripulante_plani);
	pthread_mutex_unlock(&mutex_cola_bloqueados_io);
	sem_post(&semaforo_cola_bloqueados_io);
}

t_tripulante* obtener_tripulante_de_patota(t_patota* patota, int i){
	t_tripulante* tripulante = malloc(sizeof(t_tripulante));
	tripulante->posicion = malloc(sizeof(t_posicion));

	tripulante->id_patota_asociado = patota->id_patota;
	tripulante->posicion = list_get(patota->posiciones,i);
	tripulante->id = ++cantidad_tripulantes;
	log_info(logger, "El tripulante %i esta en la posicion %i|%i", tripulante->id, tripulante->posicion->pos_x,tripulante->posicion->pos_y);
	tripulante->tarea_act = NULL;
	tripulante->estado = NEW;

	return tripulante;
}


void solicitar_tarea(t_tripulante* tripulante){
	enviar_solicitar_tarea(tripulante, tripulante->socket_conexion_RAM);
	t_tarea* tarea_por_hacer = recibir_tarea(tripulante->socket_conexion_RAM);
	if(tarea_por_hacer == NULL) {
		log_warning(logger, "Ya no hay mas tareas para el tripulante %i", tripulante->id);
	}
	tripulante->tarea_act = tarea_por_hacer;
}

void expulsar_tripulante(t_tripulante* tripulante){
	enviar_RAM_expulsar_tripulante(tripulante,tripulante->socket_conexion_RAM);

	log_info(logger, "Se expulso el tripulante %i", tripulante->id);

	free(tripulante->posicion);
	free(tripulante->tarea_act);
	liberar_conexion(&tripulante->socket_conexion_Mongo);
	liberar_conexion(&tripulante->socket_conexion_RAM);
	free(tripulante);
}

void ejecutar_tripulante(t_tripulante* tripulante){
	tripulante->socket_conexion_RAM = crear_conexion(ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ);
	tripulante->socket_conexion_Mongo = crear_conexion(ip_Mongo_Store, puerto_Mongo_Store);
	log_info(logger, "El tripulante %i tiene el socket %i con RAM", tripulante->id, tripulante->socket_conexion_RAM);

	// EL TRIPULANTE INFORMA A RAM QUE QUIERE INICIAR
	enviar_iniciar_tripulante(tripulante, tripulante->socket_conexion_RAM);
	// EL TRIPULANTE INFORMA A MONGO QUE QUIERE INICIAR
	enviar_iniciar_tripulante(tripulante, tripulante->socket_conexion_Mongo);

	// TODO :: SOLICITA LA TAREA A EJECUTAR
	solicitar_tarea(tripulante);

	while(tripulante->tarea_act && (tripulante->estado != EXIT)){
		p_tripulante* tripulante_plani = malloc(sizeof(p_tripulante));
		tripulante_plani->tripulante = tripulante;
		tripulante_plani->esta_activo = true;

		pthread_mutex_init(&tripulante_plani->mutex_solicitud, NULL);
		pthread_mutex_init(&tripulante_plani->mutex_ejecucion, NULL);
		pthread_mutex_lock(&tripulante_plani->mutex_solicitud);
		pthread_mutex_lock(&tripulante_plani->mutex_ejecucion);

		subir_tripulante_ready(tripulante_plani);

		while(quedan_pasos(tripulante) && puedo_seguir(tripulante_plani)){
			enviar_mover_hacia(tripulante, avanzar_hacia(tripulante, tripulante->tarea_act->posicion, true));
		}
		if(!puedo_seguir(tripulante_plani))
			break;
		while(tripulante->tarea_act->tiempo > 0 && puedo_seguir(tripulante_plani))
			hacer_tarea(tripulante_plani);
		if(puedo_seguir(tripulante_plani) && tripulante->tarea_act->tiempo <= 0){
			log_info(logger, "El tripulante %i finalizo la tarea %s", tripulante->id, tripulante->tarea_act->tarea);
			enviar_Mongo_bitacora_tarea_finalizar(tripulante, tripulante->socket_conexion_Mongo);
		}
		tripulante_plani->esta_activo = false;
		pthread_mutex_unlock(&tripulante_plani->mutex_solicitud);
		if(!verificar_estado(tripulante))
			break;
		solicitar_tarea(tripulante);
	}

	expulsar_tripulante(tripulante);
}

void ejecutar_tripulante_para_sabotaje(t_tripulante* tripulante){
	// Muevo al tripulante a la posicion del sabotaje
	while(!esta_en_el_lugar(tripulante->posicion, posicion_sabotaje)){
		enviar_mover_hacia(tripulante,avanzar_hacia(tripulante, posicion_sabotaje, true));
	}
}

void actualizar_estado(t_tripulante* tripulante){
	bool esta_expulsado(uint32_t id){
		return id == tripulante->id;
	}

	if(list_any_satisfy(lista_expulsados, (void*) esta_expulsado)){
		tripulante->estado = EXIT;
		log_info(logger, "El tripulante %i esta en estado EXIT", tripulante->id);
	}
}

bool verificar_estado(t_tripulante* tripulante){
	actualizar_estado(tripulante);
	return tripulante->estado != EXIT;
}

bool puedo_seguir(p_tripulante* tripulante_plani){
	pthread_mutex_unlock(&tripulante_plani->mutex_solicitud);
	pthread_mutex_lock(&tripulante_plani->mutex_ejecucion);
	verificar_sabotaje_cpu();

	return verificar_estado(tripulante_plani->tripulante);
}

void hacer_tarea(p_tripulante* tripulante_plani){
	hacer_peticon_IO();
	char* tarea_por_hacer = tripulante_plani->tripulante->tarea_act->tarea;

	log_info(logger, "El tripulante %i iniciara la tarea %s", tripulante_plani->tripulante->id, tarea_por_hacer);
	enviar_Mongo_bitacora_tarea(tripulante_plani->tripulante,tripulante_plani->tripulante->socket_conexion_Mongo);

	if(es_tarea_de_recursos(tarea_por_hacer) || es_tarea_de_eliminar_residuos(tarea_por_hacer)) {
		// TODO FS DEBE ENTERARSE QUE SE VA A TRATAR DE ACCEDER A ALGUN ARCHIVO O KE ??
		enviar_Mongo_tarea_e_s(tripulante_plani->tripulante,tripulante_plani->tripulante->socket_conexion_Mongo);
		// Se debe acceder al FS -> BLoquear al wachin hasta que termine de hacer la tarea

		tripulante_plani->esta_activo = false;
		subir_tripulante_bloqueado(tripulante_plani);
	}
	else{
		hacer_ciclos_tarea(tripulante_plani->tripulante);
	}

}

void hacer_peticon_IO() {
	rafaga_cpu(1);
}

bool es_tarea_de_recursos(char* tarea) {
	return son_iguales(tarea, "GENERAR_OXIGENO") ||
			son_iguales(tarea, "CONSUMIR_OXIGENO") ||
			son_iguales(tarea, "GENERAR_COMIDA") ||
			son_iguales(tarea, "CONSUMIR_COMIDA");
}

bool es_tarea_de_eliminar_residuos(char* tarea) {
	return son_iguales(tarea, "GENERAR_BASURA") ||
			son_iguales(tarea, "DESCARTAR_BASURA");
}

// Son distintas tareas de IO, no todas escriben sobre archivos, algunas simplemente es quedarse como salame ahi parado en EXEC
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

// TODO ESTO VA EN EL FS
bool existe_archivo(char* nombre_archivo) {
	return false;
}

void hacer_ciclos_tarea(t_tripulante* tripulante){

	if(son_iguales(algoritmo,"FIFO")){
		rafaga_cpu(tripulante->tarea_act->tiempo);
		tripulante->tarea_act->tiempo = 0;
	}
	else{
		int cuantum = minimo(tripulante->tarea_act->tiempo, quantum);
		rafaga_cpu(cuantum);
		tripulante->tarea_act->tiempo -= cuantum;
		log_info(logger, "El tripulante %i le queda %i para finalizar la tarea %s", tripulante->id, tripulante->tarea_act->tiempo, tripulante->tarea_act->tarea);
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

t_movimiento avanzar_hacia(t_tripulante* tripulante, t_posicion* posicion_meta, bool es_sabotaje) {
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
	if(!es_sabotaje)
		rafaga_cpu(1);
	else
		rafaga(1);
	log_info(logger, "El tripulante %i esta ahora en la posicion %i|%i yendo a %i|%i", tripulante->id, tripulante->posicion->pos_x,tripulante->posicion->pos_y, posicion_meta->pos_x, posicion_meta->pos_y);
	return direccion;
}


