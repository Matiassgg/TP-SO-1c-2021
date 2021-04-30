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
