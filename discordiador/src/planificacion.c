#include "planificacion.h"

void preparar_planificacion(){
	void* convertir(char* algoritmo_nombre) {
		if (son_iguales(algoritmo_nombre, "FIFO")) return (void*) planificacion_segun_FIFO;
		if (son_iguales(algoritmo_nombre, "RR")) return (void*) planificacion_segun_RR;
		return NULL;
	}

	log_info(logger, "DISCORDIADOR :: Se crearan las colas de planificacion");
	crear_colas_planificacion();
	id_tcb = 1;

	pthread_mutex_init(&mutex_cola_ready, NULL);
	pthread_mutex_init(&mutex_cola_bloqueados_io, NULL);
	sem_init(&semaforo_planificacion, 0, 0);
	sem_init(&semaforo_cola_ready, 0, 0);
	planificacion_corto_plazo = convertir(algoritmo);

	if(algoritmo == NULL)
		log_error(logger, "Error al leer el algoritmo de planificacion");
	else if(son_iguales(algoritmo, "RR"))
		log_info(logger, "El algoritmo seleccionado es RR con quantum de %d", quantum);
	else if(son_iguales(algoritmo, "FIFO"))
		log_info(logger, "El algoritmo seleccionado es FIFO");
	else
		log_info(logger, "El algoritmo seleccionado no coincide con ningun algoritmo valido");


}

void planificar_patota(t_patota* patota){
	log_info(logger, "DISCORDIADOR :: Crearemos los tripulantes de la patota");
	for(int i=0; i < patota->cant_tripulantes; i++){
		t_tripulante* tripulante = obtener_tripulante_de_patota(patota, i);
		log_info(logger, "DISCORDIADOR :: Se crea el tripulante %i", tripulante->id);
		pthread_t hiloTripulante;
		pthread_create(&hiloTripulante, NULL, (void*) ejecutar_tripulante, tripulante);
		pthread_detach(hiloTripulante);
	}


}
void arrancar_planificacion(){
	for(int i=0; i<grado_multitarea; i++){
		log_info(logger, "Se entro al for de iniciar plani");
		pthread_t planificar_corto;
		pthread_create(&planificar_corto, NULL, (void*) planificacion_corto_plazo, NULL);
		pthread_detach(planificar_corto);
	}
}

void iniciar_planificacion(){
	for(int i=0; i<grado_multitarea; i++){
		sem_post(&semaforo_planificacion);
		log_info(logger, "Se inicia plani");
	}
}

void pausar_planificacion(){
	for(int i=0; i<grado_multitarea; i++){
		sem_wait(&semaforo_planificacion);
		log_info(logger, "Se pausa plani");
	}
}

void planificacion_segun_FIFO() {
	log_info(logger, "Entramos a la planificacion POR FIFO");
	while(1){
		verificar_planificacion_activa();

		while (!cola_ready_vacia()) {
			pthread_mutex_lock(&mutex_cola_ready);
			p_tripulante* tripulante_plani = (p_tripulante*) queue_pop(cola_ready);
			pthread_mutex_unlock(&mutex_cola_ready);
			tripulante_plani->tripulante->estado = EXEC;

			// TODO :: AVISAR A RAM QUE AHORA ESTA EN EXEC


			while(verificar_planificacion_activa() && tripulante_plani->esta_activo){
				pthread_mutex_lock(&tripulante_plani->mutex_solicitud);
				if(tripulante_plani->esta_activo)
					pthread_mutex_unlock(&tripulante_plani->mutex_ejecucion);
			}
		}
	}
}

bool cola_ready_vacia(){
	sem_wait(&semaforo_cola_ready);
	pthread_mutex_lock(&mutex_cola_ready);
	bool esta_vacia = queue_is_empty(cola_ready);
	pthread_mutex_unlock(&mutex_cola_ready);

	return esta_vacia;
}

bool verificar_planificacion_activa(){
	sem_wait(&semaforo_planificacion);
	sem_post(&semaforo_planificacion);

	return true;
}

void planificacion_segun_RR() {
	log_info(logger, "Entramos a la planificacion POR RR");

	while(1){
		verificar_planificacion_activa();

		while (!cola_ready_vacia()) {
			pthread_mutex_lock(&mutex_cola_ready);
			p_tripulante* tripulante_plani = (p_tripulante*) queue_pop(cola_ready);
			pthread_mutex_unlock(&mutex_cola_ready);
//			t_tripulante* tripulante = tripulante_plani->tripulante;
			// tripulante->estado = EXEC;

			for(int i=0; i<quantum && verificar_planificacion_activa() && tripulante_plani->esta_activo; i++) {
				pthread_mutex_lock(&tripulante_plani->mutex_solicitud);
				if(tripulante_plani->esta_activo)
					pthread_mutex_unlock(&tripulante_plani->mutex_ejecucion);
			}
			if(tripulante_plani->esta_activo)
				subir_tripulante_ready(tripulante_plani);
		}
	}
}

void finalizar_tripulante_plani(uint32_t id_tripulante) {
	list_add(lista_expulsados, id_tripulante);
//	t_tripulante* tripulante_por_expulsar = malloc(sizeof(t_tripulante));
//	free(tripulante_por_expulsar);
	// TODO NO ES ASI
}

void crear_colas_planificacion() {
	cola_new = queue_create();
	cola_ready = queue_create();
	cola_exec = queue_create();
	cola_exit = queue_create();
	cola_bloq_E_S = queue_create();
	cola_bloq_Emergencia = queue_create();
	lista_expulsados = list_create();
}

void liberar_pcb_patota(t_patota* patota){
	list_destroy_and_destroy_elements(patota->posiciones, free);
	free(patota);
}
