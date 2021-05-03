#include "planificacion.h"

void preprar_planificacion(){
	log_info(logger, "DISCORDIADOR :: Se crearan las colas de planificacion");
	crear_colas_planificacion();
	id_tcb=1;

	if(algoritmo == NULL)
		log_info(logger, "Error al leer el algoritmo de planificacion");
	else if(son_iguales(algoritmo, "RR"))
		log_info(logger, "El algoritmo seleccionado es: %s con quantum de %d", algoritmo, quantum);
	else
		log_info(logger, "El algoritmo seleccionado es: %s", algoritmo);
}

void planificar_patota(t_patota* patota){
	log_info(logger, "DISCORDIADOR :: Crearemos los tripulantes de la patota");
//	LA RE FLASHE PARECE QUE PARA PLANI ESTOS VAN A ESTAR PIDIENDO T0DO A MEMORIA UNA PAJA SI ES ASI UwU
//	t_pcb* pcb_nuevo = crear_pcb(patota);
//	t_list* lista_tcbs_aux = agregar_tcbs(patota);

	for(int i=0; i < patota->cant_tripulantes; i++){
		t_tripulante* tripulante = obtener_tripulante_de_patota(patota, i);
		log_info(logger, "DISCORDIADOR :: Se crea el tripulante %i", tripulante->id);
		pthread_t hiloTripulante;
		pthread_create(&hiloTripulante, NULL, (void*) iniciar_tripulante, tripulante);
		pthread_detach(hiloTripulante);
	}


}

t_tripulante* obtener_tripulante_de_patota(t_patota* patota, int i){
	t_tripulante* tripulante = malloc(sizeof(t_tripulante));

	tripulante->id_patota_asociado = patota->id_patota;
	tripulante->posicion = list_get(patota->posiciones,i);
	tripulante->id = i;
	tripulante->tarea_act = NULL;

	return tripulante;
}

void iniciar_tripulante(t_tripulante* tripulante){
	int socket_RAM = crear_conexion(ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ);

	enviar_iniciar_tripulante(tripulante, socket_RAM);

//	tripulante->tarea_act = solicitar_tarea(tripulante);
	queue_push(cola_new, tripulante);
}

t_pcb* crear_pcb(t_patota* patota){
	log_info(logger, "DISCORDIADOR :: Se crea el PCB para la patota %d", patota->id_patota);
	t_pcb* pcb = malloc(sizeof(t_pcb));

	pcb->tareas = 0; // cosa de ram
	pcb->pid = patota->id_patota;

//	list_add(lista_tcbs, pcb);
	return pcb;
}

t_list* agregar_tcbs(t_patota* patota){
	t_list* lista_tcbs_aux = list_create();
	for(int i=0; i < patota->cant_tripulantes; i++){
		log_info(logger, "DISCORDIADOR :: Se crea el TCB para el tripualante %d de la patota %d", i, patota->id_patota);

		t_tcb* tcb = malloc(sizeof(t_tcb));

		tcb->tid = id_tcb++; // empieza en 1 y va a ir subiendo
		tcb->estado = NEW;
		tcb->prox_instruccion = 0; // cosa de ram
		tcb->posicion = (t_posicion*) list_get(patota->posiciones, i);
		tcb->puntero_pcb = 0; // cosa de ram

		log_info(logger, "DISCORDIADOR :: Se creo el TCB en NEW, para el tripulante: %d", tcb->tid);

//		list_add(lista_tcbs, tcb);
		list_add(lista_tcbs_aux, tcb);
		queue_push(cola_new, tcb);
	}

	return lista_tcbs_aux;
}

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
