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
	log_info(logger, "Se entro a FIFO");
	while(1){
		verificar_planificacion_activa();

		pthread_mutex_lock(&mutex_cola_ready);
		while (!queue_is_empty(cola_ready)) {
			log_info(logger, "Se entro al while");
			p_tripulante* tripulante_plani = (p_tripulante*) queue_pop(cola_ready);
			pthread_mutex_unlock(&mutex_cola_ready);
			t_tripulante* tripulante = tripulante_plani->tripulante;
	//		tripulante->estado = EXEC; // avisar a ram?

			while(verificar_planificacion_activa() && tripulante_plani->esta_activo){
				pthread_mutex_unlock(&tripulante_plani->mutex_ready);
			}
//			verificar_planificacion_activa();
//			hacer_tarea(tripulante);
			// Avisar a ram que se esta haciendo la tarea

			//FALTA SEGUIR :V
		}
	}
}

bool verificar_planificacion_activa(){
	sem_wait(&semaforo_planificacion);
	sem_post(&semaforo_planificacion);

	return true;
}

void planificacion_segun_RR() {
	sem_wait(&semaforo_planificacion);
	log_info(logger, "Se entro a RR");

	while(1){
		uint32_t quantum_de_config = quantum;

		while (!queue_is_empty(cola_ready)) {
			t_tripulante* tripulante = (t_tripulante*) queue_pop(cola_ready);
		//		tripulante->estado = EXEC;

			while(quantum_de_config > 0){

				if(quedan_pasos(tripulante)) {
					enviar_mover_hacia(tripulante, avanzar_hacia(tripulante, tripulante->tarea_act->posicion));
				}
				else {
					// Si le dio el quantum para llegar a la posicion donde debe hacer la tarea
					hacer_tarea(tripulante);
					// Avisar a ram que se esta haciendo la tarea
				}

				quantum_de_config--;
			}
			if(quedan_pasos(tripulante))
				queue_push(cola_ready, tripulante);	// Vuelve a la cola de ready por fin de Q
			quantum_de_config = quantum;
			// TODO
		}
	}
	sem_post(&semaforo_planificacion);

}
//
//t_pcb* crear_pcb(t_patota* patota){
//	log_info(logger, "DISCORDIADOR :: Se crea el PCB para la patota %d", patota->id_patota);
//	t_pcb* pcb = malloc(sizeof(t_pcb));
//
//	pcb->tareas = 0; // cosa de ram
//	pcb->pid = patota->id_patota;
//
////	list_add(lista_tcbs, pcb);
//	return pcb;
//}
//
//t_list* agregar_tcbs(t_patota* patota){
//	t_list* lista_tcbs_aux = list_create();
//	for(int i=0; i < patota->cant_tripulantes; i++){
//		log_info(logger, "DISCORDIADOR :: Se crea el TCB para el tripualante %d de la patota %d", i, patota->id_patota);
//
//		t_tcb* tcb = malloc(sizeof(t_tcb));
//
//		tcb->tid = id_tcb++; // empieza en 1 y va a ir subiendo
//		tcb->estado = NEW;
//		tcb->prox_instruccion = 0; // cosa de ram
//		tcb->posicion = (t_posicion*) list_get(patota->posiciones, i);
//		tcb->puntero_pcb = 0; // cosa de ram
//
//		log_info(logger, "DISCORDIADOR :: Se creo el TCB en NEW, para el tripulante: %d", tcb->tid);
//
////		list_add(lista_tcbs, tcb);
//		list_add(lista_tcbs_aux, tcb);
//		queue_push(cola_new, tcb);
//	}
//
//	return lista_tcbs_aux;
//}

void crear_colas_planificacion() {
	cola_new = queue_create();
	cola_ready = queue_create();
	cola_exec = queue_create();
	cola_exit = queue_create();
	cola_bloq_E_S = queue_create();
	cola_bloq_Emergencia = queue_create();
}

void liberar_pcb_patota(t_patota* patota){
	list_destroy_and_destroy_elements(patota->posiciones, free);
	free(patota);
}
