#include "planificacion.h"

void iniciar_planificacion(){
	cola_new = queue_create();
	cola_ready = queue_create();
	cola_exec = queue_create();
	cola_exit = queue_create();
	cola_bloq_E_S = queue_create();
	cola_bloq_Emergencia = queue_create();

	if(algoritmo == NULL)
		log_info(logger, "Error al leer el algoritmo de planificacion");
	else if(son_iguales(algoritmo, "RR"))
		log_info(logger, "El algoritmo seleccionado es: %s con quantum de %d", algoritmo, quantum);
	else
		log_info(logger, "El algoritmo seleccionado es: %s", algoritmo);
}

void planificar_patota(t_patota* patota){
	t_pcb* pcb_nuevo = crear_pcb(patota);
}

t_pcb* crear_pcb(t_patota* patota){
	t_pcb* pcb = malloc(sizeof(t_pcb));

	pcb->tareas = patota->path_tareas; //????? asi o hay que hacer como una lista de funciones???
	pcb->pid = patota->id_patota;

	return pcb;
}

t_list* crear_tcbs(t_patota* patota){
	t_list* lista_tcbs = list_create();
	for(int i=0; i<patota->cant_tripulantes; i++){
		t_tcb* tcb = malloc(sizeof(t_pcb));

		tcb->tid = i; //????
		tcb->estado = NEW;
		tcb->prox_instruccion = i; //?????
		tcb->posicion = list_get(patota->posiciones, i);

		list_add(lista_tcbs, tcb);
	}

	return lista_tcbs;
}
