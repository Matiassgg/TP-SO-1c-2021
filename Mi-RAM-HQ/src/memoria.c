#include "memoria.h"

void iniciar_memoria() {
	// Reservar el espacio de memoria
	preparar_memoria();

//	// FALTA
//	// Dibujar el mapa inicial vacío
//	pthread_create(&hiloReceiveMapa, NULL, (void*) iniciar_mapa_vacio, NULL);
//	pthread_detach(hiloReceive);

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

t_pcb* crear_pcb(t_patota* patota){
	log_info(logger, "RAM :: Se crea el PCB para la patota %d", patota->id_patota);
	t_pcb* pcb = malloc(sizeof(t_pcb));

	pcb->tareas = 0;
	pcb->pid = patota->id_patota;

	return pcb;
}

t_tcb* crear_tcbs(t_tripulante* tripulante){
	log_info(logger, "RAM :: Se crea el TCB para el tripualante %d de la patota %d", tripulante->id, tripulante->id_patota_asociado);
	t_tcb* tcb = malloc(sizeof(t_tcb));

	tcb->tid = tripulante->id;
	tcb->estado = 'N';
	tcb->prox_instruccion = 0;
	tcb->posicion = tripulante->posicion;
	tcb->puntero_pcb = 0;

	log_info(logger, "RAM :: Se creo el TCB en NEW, para el tripulante: %d", tcb->tid);

	return tcb;
}


void preparar_memoria_para_esquema_de_segmentacion() {
	tabla_segmentos = list_create();
	t_segmento* segmento = malloc(sizeof(t_segmento));

	segmento->inicio = 0;
	segmento->nro_segmento = 0;
	segmento->tamanio = tamanio_memoria;
	segmento->esta_libre = true;

	list_add(tabla_segmentos, segmento);

}


void preparar_memoria_para_esquema_de_paginacion() {
	cantidad_de_marcos = 0;
	cantidad_de_marcos_swap=0;

	if((espacio_swap = fopen(path_swap,"wb+")) == NULL){
		log_error(logger, "No se pudo crear el espacio de SWAP");
//		return; taria bien?
	}

	for(int offset = 0; offset < tamanio_memoria -1; offset += tamanio_pagina){
		// Cargar estructuras administrativass
		log_info(logger, "Desplazamiento: %d", offset);
		void* marco = memoria + offset;

		entradaTablaMarcos* nuevaEntrada = malloc(sizeof(entradaTablaMarcos));
		nuevaEntrada->indice = cantidad_de_marcos;
		nuevaEntrada->bitModificado = false;
		nuevaEntrada->bitUso = false;
		nuevaEntrada->idPatota = -1;
		nuevaEntrada->libre = true;
		nuevaEntrada->marco = marco;
		nuevaEntrada->discordiador = NULL;
		nuevaEntrada->timeStamp = NULL;

		list_add(tablaDeMarcos, nuevaEntrada);
		log_info(logger, "Marco numero: %d", nuevaEntrada->indice);
		cantidad_de_marcos++;
	}

	int indice=0;

	// Aca viene la parte de swap
	for(int offset = 0; offset < tamanio_swap -1; offset += tamanio_pagina){
		void* marco = memoria + offset;

		entradaSwap* nuevaEntrada = malloc(sizeof(entradaSwap));
		nuevaEntrada->idPatota = -1;
		nuevaEntrada->libre = true;
		nuevaEntrada->indiceMarcoSwap = indice;
		indice ++;

		list_add(entradas_swap, nuevaEntrada);
		cantidad_de_marcos_swap++;
	}

	log_info(logger, "Cantidad de Marcos Libres: %d", cantidad_de_marcos);
	log_info(logger, "Cantidad de Marcos en SWAP: %d",cantidad_de_marcos_swap);

}

uint32_t escribir_en_memoria(void* informacion, e_tipo_dato tipo_dato){
	switch(tipo_dato){
		case TAREAS:
			if(son_iguales(esquema_memoria, "SEGMENTACION")) {
				return escribir_en_memoria_segmentacion(serializar_memoria_tareas((char*) informacion));
			}
			else{

			}
		break;
		case PCB:
			;
			t_pcb* pcb = (t_pcb*) informacion;

			if(son_iguales(esquema_memoria, "SEGMENTACION")) {
				return escribir_en_memoria_segmentacion(serializar_memoria_pcb(pcb));
			}
			else{

			}
		break;
		case TCB:
			;
			t_tcb* tcb = (t_tcb*) informacion;

			if(son_iguales(esquema_memoria, "SEGMENTACION")) {
				return escribir_en_memoria_segmentacion(serializar_memoria_tcb(tcb));
			}
			else{

			}
		break;

	}

	return 0;
}

uint32_t escribir_en_memoria_segmentacion(t_buffer* buffer){
	t_segmento* segmento_libre = buscar_segmento_libre(buffer->size);
	t_segmento* segmeto_nuevo = dar_nuevo_segmento(segmento_libre, buffer->size);

	segmento_libre->esta_libre = false;
	segmento_libre->tamanio = buffer->size;

	subir_segmento(segmento_libre, buffer->stream);
	subir_segmento(segmeto_nuevo, buffer->stream);

	return segmento_libre->inicio;
}

t_segmento* dar_nuevo_segmento(t_segmento* segmento, uint32_t size){
	t_segmento* segmento_nuevo = malloc(sizeof(t_segmento));

	segmento_nuevo->esta_libre = true;
	segmento_nuevo->inicio =+ size;
	segmento_nuevo->tamanio =- size;
	segmento_nuevo->nro_segmento++; // TODO ESTO NO ES ASI, ARREGLAR

	return segmento_nuevo;
}

void subir_segmento(t_segmento* segmento, void* stream){
	bool ordenar_segmentos(t_segmento* segmento_1, t_segmento* segmento_2){
		return (segmento_1->nro_segmento < segmento_2->nro_segmento);
	}

	if(segmento->esta_libre)
		;
	else
		memcpy(memoria + segmento->inicio, &(stream), segmento->tamanio);

	list_add(tabla_segmentos, segmento);
	list_sort(tabla_segmentos, ordenar_segmentos);
}

t_segmento* buscar_segmento_libre(uint32_t espacio_requerido){
	bool esta_libre(t_segmento* segmento){
		return (segmento->esta_libre && (espacio_requerido <= segmento->tamanio));
	}

	return list_remove_by_condition(tabla_segmentos, esta_libre);
}

void escribir_en_memoria_paginacion(/*va a tener que ser una pagina*/t_pcb* tcb, bool esta_en_memoria, uint32_t idPedido, bool modificado) {
//	entradaTablaMarcos* entradaDeLaTabla;
//
//	if (esta_en_memoria) {
//
//		// Busco el marco de ese plato/pagina
//		t_list* entradasDelPedido = entradas_segun_pedido(idPedido);
//
//		int cantidadMarcos = list_size(entradasDelPedido);
//		bool encont = false;
//		entradaTablaMarcos* entrada;
//
//		for (int i = 0; i < cantidadMarcos && !encont; i++) {
//			entradaDeLaTabla = list_get(entradasDelPedido, i);
//			int offset = 8;
//			char* nombreEnMarco = malloc(24);
//			memcpy(nombreEnMarco, (entradaDeLaTabla->marco + offset), 24);
//
//			if (!strcmp(pagina->nombrePlato, nombreEnMarco))
//				encont = true;
//			free(nombreEnMarco);
//		}
//
//		if (!strcmp(algoritmo_reemplazo, "LRU")) {
//			entradaDeLaTabla->timeStamp = temporal_get_string_time();
//		}
//
//		if (!strcmp(algoritmo_reemplazo, "CLOCK_ME")) {
//			entradaDeLaTabla->bitUso = true;
//			entradaDeLaTabla->bitModificado = modificado;
//		}
//	} else {
//		entradaDeLaTabla = asignar_entrada_marco_libre();
//		entradaDeLaTabla->libre = false;
//		entradaDeLaTabla->idPedido = idPedido;
//
//		log_info(logger, "Marco seleccionado: %d", entradaDeLaTabla->indice);
//
//		// Agrego a lista de timestamp por marco
//		if (!strcmp(algoritmo_reemplazo, "LRU")) {
//			entradaDeLaTabla->timeStamp = temporal_get_string_time();
//			log_info(logger, "El timestamp del marco numero %d se ha inicializado: %s", entradaDeLaTabla->indice, entradaDeLaTabla->timeStamp);
//		}
//
//		// Agrego a lista de timestamp por marco
//		if (!strcmp(algoritmo_reemplazo, "CLOCK_ME")) {
//			entradaDeLaTabla->bitModificado = modificado;
//			entradaDeLaTabla->bitUso = true;
//			log_info(logger, "Los bits del marco numero %d se han inicializados: bit de uso: %d - bit de modificado %d .", entradaDeLaTabla->indice, entradaDeLaTabla->bitUso, entradaDeLaTabla->bitModificado);
//		}
//
////		// Agrego el marco a la lista de marcos del pedido
////		t_list* marcos_del_pedido_en_memoria = entradas_segun_pedido(idPedido);
////		list_add(marcos_del_pedido_en_memoria, marco_libre);
////		log_info(logger, "Se agrego el marco a la lista de marcos del pedido");
//	}
//
//	int offset = 0;
//
//	// Escribo
//	memcpy(entradaDeLaTabla->marco + offset, &(pagina->cantidadLista), sizeof(uint32_t));
//	offset += sizeof(uint32_t);
//
//	memcpy(entradaDeLaTabla->marco + offset, &(pagina->cantidadPlato), sizeof(uint32_t));
//	offset += sizeof(uint32_t);
//
//	memcpy(entradaDeLaTabla->marco + offset, pagina->nombrePlato, 24);
//
//	log_info(logger, "Escrito con exito");
}
entradaTablaMarcos* buscar_entrada(void* marco){
	bool el_que_quiero(void* parametro){
		return ((entradaTablaMarcos*)parametro)->marco == marco;
	}

	return list_find(tablaDeMarcos, el_que_quiero);
}

entradaTablaMarcos* asignar_entrada_marco_libre(void){
	bool este_libre(void* parametro){
		return ((entradaTablaMarcos*)parametro)->libre;
	}

	if (!hay_marcos_libres()) {
		log_info(logger, "No hay marcos libres. Seleccionando victima.");

		pthread_t hilo;

		pthread_mutex_lock(&mutexVictima);
		pthread_create(&hilo, NULL, (void*) seleccionar_victima, NULL);
		pthread_join(hilo, 0);
		pthread_mutex_unlock(&mutexVictima);
	}

	t_list* entradas_marcos_libres = list_filter(tablaDeMarcos, este_libre);

	entradaTablaMarcos* buscado = list_get(entradas_marcos_libres, 0);

	list_destroy(entradas_marcos_libres);

	return buscado;
}

void* convertir(char* algoritmo_nombre) {
	if (!strcmp(algoritmo_nombre, "LRU")) return (void*) seleccionar_victima_LRU;
	if (!strcmp(algoritmo_nombre, "CLOCK")) return (void*) seleccionar_victima_CLOCK;

	return NULL;
	}

void seleccionar_victima_LRU(void){
	bool mas_viejo(void* unElem, void* otroElem){
		entradaTablaMarcos* elem1 = unElem;
		entradaTablaMarcos* elem2 = otroElem;

		char ** timeElem1;
		char ** timeElem2;

		int horaElem1;
		int horaElem2;
		int minElem1;
		int minElem2;
		int segElem1;
		int segElem2;
		int milElem1;
		int milElem2;

		timeElem1 = string_split(elem1->timeStamp, ":");
		timeElem2 = string_split(elem2->timeStamp, ":");

		horaElem1 = atoi(timeElem1[0]);
		horaElem2 = atoi(timeElem2[0]);

		minElem1 = atoi(timeElem1[1]);
		minElem2 = atoi(timeElem2[1]);

		segElem1 = atoi(timeElem1[2]);
		segElem2 = atoi(timeElem2[2]);

		milElem1 = atoi(timeElem1[3]);
		milElem2 = atoi(timeElem2[3]);

		if (horaElem1 < horaElem2) {
			return horaElem1 < horaElem2;
		} else if (horaElem1 == horaElem2) {
			if (minElem1 < minElem2) {
				return minElem1 < minElem2;
			} else if (minElem1 == minElem2) {
				if (segElem1 < segElem2) {
					return segElem1 < segElem2;
				} else if (segElem1 == segElem2) {
					if (milElem1 <= milElem2) {
						return milElem1 <= milElem2;
					}
				}
			}
		}
	}


	pthread_mutex_lock(&mutexFree);

	list_sort(tablaDeMarcos, mas_viejo);
	entradaTablaMarcos* entrada_mas_vieja = list_get(tablaDeMarcos, 0);

	log_info(logger, "Victima seleccionada: %d", entrada_mas_vieja->indice);

	//todo FALTA VER QUE PASA CON SWAP ACA

	entrada_mas_vieja->libre = true;

	pthread_mutex_unlock(&mutexFree);
}

void seleccionar_victima_CLOCK(void){
	int siguiente_posicion(int* posicion_actual){
		return (*posicion_actual + 1) % cantidad_de_marcos;
	}

	if(punteroMarcoClock == NULL){
		// Apunta a la primer entrada de la Tabla de Marcos
		punteroMarcoClock = list_get(tablaDeMarcos, 0);
	}

	int posicion_puntero_actual = indice_elemento(tablaDeMarcos, punteroMarcoClock);

	bool victima_seleccionada = false;

	while (!victima_seleccionada) {
		if (!victima_seleccionada) {
			for (int i = 0; i < cantidad_de_marcos && !victima_seleccionada; i++) {
				if (bit_uso_apagado(punteroMarcoClock)) victima_seleccionada = true;
				if(!victima_seleccionada){
					// referencio a la siguiente entrada
					punteroMarcoClock = list_get(tablaDeMarcos, ((posicion_puntero_actual + 1) % cantidad_de_marcos));

					// aumento el indice del puntero
					posicion_puntero_actual = ((posicion_puntero_actual + 1) % cantidad_de_marcos);
				}
			}
		}
/* CREO QUE ESTO NO HACE FALTA, CONSULTAR CON LOS PIBARDOS

		if (!victima_seleccionada) {
			for (int i = 0; i < cantidad_de_marcos && !victima_seleccionada; i++) {
				if (!bit_uso(punteroMarcoClock) && bit_modificado(punteroMarcoClock)) {
					victima_seleccionada = true;
				} else {
					punteroMarcoClock->bitUso = false;
				}
				if(!victima_seleccionada){
					// referencio a la siguiente entrada
					punteroMarcoClock = list_get(tablaDeMarcos, ((posicion_puntero_actual + 1) % cantidad_de_marcos));

					// aumento el indice del puntero
					posicion_puntero_actual = ((posicion_puntero_actual + 1) % cantidad_de_marcos);
				}
			}
		}
*/
	}


	pthread_mutex_lock(&mutexFree);

	log_info(logger, "Victima seleccionada: %d", punteroMarcoClock->indice);

	// todo Escribo en SWAP

	((entradaTablaMarcos*)list_get(tablaDeMarcos, posicion_puntero_actual))->libre = true;

	pthread_mutex_unlock(&mutexFree);
}


bool bit_uso_apagado(entradaTablaMarcos* entrada){
	return !entrada->bitUso;
}

bool bit_uso(entradaTablaMarcos* entrada){
	return entrada->bitUso;
}

int indice_elemento(t_list* lista, void* elemento){
	int indice = 0;
	bool encontrado = false;

	for (int i = 0; i < list_size(lista) && !encontrado; i++) {
		if(list_get(lista, i) == elemento) encontrado = true;
		indice = i;
	}

	return indice;
}

bool hay_marcos_libres(void){
	bool este_libre(void* parametro){
		return ((entradaTablaMarcos*)parametro)->libre;
	}

	t_list* marcos_libres = list_filter(tablaDeMarcos, este_libre);

	int tamanio = list_size(marcos_libres);

	list_destroy(marcos_libres);

	return tamanio > 0;
}


