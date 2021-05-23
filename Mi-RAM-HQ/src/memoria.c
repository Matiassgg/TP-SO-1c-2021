#include "memoria.h"

void iniciar_memoria() {
	// Reservar el espacio de memoria
	preparar_memoria();

	// TODAVIA no es necesario esto

	if((espacio_swap = fopen(path_swap,"wb+")) == NULL){
		log_error(logger, "No se pudo crear el espacio de SWAP");
		return;
	}

	// FALTA
	// Dibujar el mapa inicial vacío
	pthread_create(&hiloReceiveMapa, NULL, (void*) iniciar_mapa_vacio, NULL);
	pthread_detach(hiloReceive);

}

void preparar_memoria() {
	memoria = malloc(tamanio_memoria);

	if(esquema_memoria == NULL)
		log_error(logger, "Error al leer el esquema de memoria");
	else if(son_iguales(esquema_memoria, "SEGMENTACION")) {
		log_info(logger, "El esquema de memoria seleccionado es SEGMENTACION PURA");
		preparar_memoria_para_esquema_de_segmentacion();
	}
	else if(son_iguales(esquema_memoria, "PAGINACION")) {
		log_info(logger, "El esquema de memoria seleccionado es PAGINACION SIMPLE CON MEMORIA VIRTUAL");
		preparar_memoria_para_esquema_de_paginacion();
	}

	else
		log_warning(logger,"El esquema de memoria seleccionado no coincide con ningun esquema");

	if(algoritmo_reemplazo == NULL)
		log_error(logger, "Error al leer el algoritmo de reemplazo");
	else if(son_iguales(algoritmo_reemplazo, "LRU"))
		log_info(logger, "El algoritmo de reemplazo seleccionado es LRU");
	else if(son_iguales(algoritmo_reemplazo, "CLOCK"))
		log_info(logger, "El algoritmo de reemplazo seleccionado es CLOCK");
	else
		log_warning(logger,"El algoritmo de reemplazo seleccionado no coincide con ningun algoritmo valido");



}

void preparar_memoria_para_esquema_de_segmentacion() {

	for(int offset = 0; offset < tamanio_memoria -1; offset += tamanio_pagina){
		// Cargar estructuras administrativass
	}
}


void preparar_memoria_para_esquema_de_paginacion() {
	int cantidad_de_marcos = 0;

	for(int offset = 0; offset < tamanio_memoria -1; offset += tamanio_pagina){
		// Cargar estructuras administrativass
		// entradaATablaDEMarcos* nuevaEntrada = malloc(sizeof(******));


		// list_add(tablaDeMarcos, nuevaEntrada);
		// log_info(logger, "SE agrega el marco numero: %d", nuevaEntrada->****);
		cantidad_de_marcos++;

		void* marco = memoria + offset;
	}

	log_info(logger, "Cantidad de Marcos Libres: %d", cantidad_de_marcos);


	// Aca viene la parte de swap
	for(int offset = 0; offset < tamanio_swap -1; offset += tamanio_pagina){
		void* marco = memoria + offset;


	}




}

