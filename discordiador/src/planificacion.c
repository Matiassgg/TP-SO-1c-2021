#include "planificacion.h"

void iniciar_planificacion(void){
	cola_new = queue_create();
	cola_ready = queue_create();
	cola_exec = queue_create();
	cola_exit = queue_create();
	cola_bloq = queue_create();
}
