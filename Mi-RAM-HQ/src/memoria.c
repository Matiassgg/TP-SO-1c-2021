#include "memoria.h"

void iniciar_memoria() {
	// Reservar el espacio de memoria
	preparar_memoria();

//	// FALTA
	// Dibujar el mapa inicial vacío
	pthread_create(&hiloReceiveMapa, NULL, (void*) iniciar_mapa_vacio, NULL);
	pthread_detach(hiloReceiveMapa);
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

	    // Leer Algoritmo Seleccion de Victima
	    if ((seleccionar_victima = convertir(algoritmo_reemplazo)) == NULL)
	    	log_warning(logger, "Error al leer el algoritmo de Reemplazo");
	    else
	    	log_info(logger, "El algoritmo de reemplazo seleccionado es: %s", algoritmo_reemplazo);
	}
	else
		log_warning(logger,"El esquema de memoria seleccionado no coincide con ningun esquema");

}

t_tabla_segmentos* crear_tabla(uint32_t id_patota){
	log_info(logger, "RAM :: Se crea la tabla para la patota %d", id_patota);
	t_tabla_segmentos* tabla = malloc(sizeof(t_tabla_segmentos));
	tabla->diccionario_segmentos = dictionary_create();

	tabla->id_patota_asociada = id_patota;
	tabla->tareas_dadas = 0;
	pthread_mutex_init(&tabla->mutex_obtener_tareas, NULL);


	pthread_mutex_lock(&mutex_tablas_segmentos);
	list_add(lista_tablas_segmentos, tabla);
	pthread_mutex_unlock(&mutex_tablas_segmentos);

	return tabla;
}
t_pcb* crear_pcb(t_patota* patota){
	log_info(logger, "RAM :: Se crea el PCB para la patota %d", patota->id_patota);
	t_pcb* pcb = malloc(sizeof(t_pcb));

	pcb->tareas = 0;
	pcb->pid = patota->id_patota;

	return pcb;
}

void cargar_memoria_patota(t_patota* patota){
	t_pcb* pcb_nuevo = crear_pcb(patota);

	char* tareas = obtener_tareas(pcb_nuevo);

	if(son_iguales(esquema_memoria, "SEGMENTACION")) {
		t_tabla_segmentos* tabla_nueva = crear_tabla(pcb_nuevo->pid);

		pthread_mutex_lock(&mutex_tocar_memoria);
		escribir_en_memoria(tareas, pcb_nuevo->pid, TAREAS);
		pthread_mutex_unlock(&mutex_tocar_memoria);
		log_info(logger, "Se subio a memoria la tarea");

		t_segmento* segmento_tareas = buscar_segmento_id(pcb_nuevo->pid, pcb_nuevo->pid, TAREAS);
		pcb_nuevo->tareas = segmento_tareas->inicio;

		pthread_mutex_lock(&mutex_tocar_memoria);
		escribir_en_memoria(pcb_nuevo, pcb_nuevo->pid, PCB);
		pthread_mutex_unlock(&mutex_tocar_memoria);
		log_info(logger, "Se subio a memoria la patota");

	}
	else{
		//revisar toda esta cagada que hice para empezar algo
		if(son_iguales(esquema_memoria, "PAGINACION")){
			t_tabla_paginas* tabla_pagina_patota = malloc(sizeof(t_tabla_paginas));
			tabla_pagina_patota->id_patota_asociada = pcb_nuevo->pid;
			tabla_pagina_patota->paginas = list_create();
			tabla_pagina_patota->tareas_dadas = 0;

			pthread_mutex_lock(&mutex_tocar_memoria);
			escribir_en_memoria(tareas, pcb_nuevo->pid, TAREAS);
			pthread_mutex_unlock(&mutex_tocar_memoria);
			log_info(logger, "Se subio a memoria la tarea");

//			t_pagina* pagina_tareas = buscar_pagina_id(nro_pagina_tareas,TAREAS);
//			pcb_nuevo->tareas = pagina_tareas->inicio; TODO ??

			pthread_mutex_lock(&mutex_tocar_memoria);
			escribir_en_memoria(pcb_nuevo, pcb_nuevo->pid, PCB);
			pthread_mutex_unlock(&mutex_tocar_memoria);
			log_info(logger, "Se subio a memoria la patota");

//			t_pagina* pagina_pcb = buscar_pagina_id(nro_pagina_patota, PCB);

//			list_add(tabla_pagina_patota->paginas, pagina_tareas);
//			list_add(tabla_pagina_patota->paginas, pagina_pcb);
//			list_add(lista_tablas_segmentos, tabla_segmento_patota); TODO SEGMENTOS?
		}

	}

}

uint32_t obtener_direccion_pcb(uint32_t id_patota){
	t_segmento* segmento = buscar_segmento_id(id_patota,id_patota,PCB);

	return segmento->inicio;
}

t_tcb* crear_tcb(t_tripulante* tripulante){
	log_info(logger, "RAM :: Se crea el TCB para el tripualante %d de la patota %d", tripulante->id, tripulante->id_patota_asociado);
	t_tcb* tcb = malloc(sizeof(t_tcb));
	tcb->posicion = malloc(sizeof(t_posicion));

	tcb->tid = tripulante->id;
	tcb->estado = 'N'; // TODO
	tcb->prox_instruccion = 0;
	tcb->posicion->pos_x = tripulante->posicion->pos_x;
	tcb->posicion->pos_y = tripulante->posicion->pos_y;
	tcb->puntero_pcb = obtener_direccion_pcb(tripulante->id_patota_asociado); // TODO CAMBIAR ESTA KK SI SE ARREGLA ESTO SE FACILITA EL DAR TAREAS

	log_info(logger, "RAM :: Se creo el TCB en NEW, para el tripulante: %d", tcb->tid);

	return tcb;
}

void cargar_memoria_tripulante(t_tripulante* tripulante){
	t_tcb* tcb = crear_tcb(tripulante);

	if(son_iguales(esquema_memoria, "SEGMENTACION")) {
		pthread_mutex_lock(&mutex_tocar_memoria);
		escribir_en_memoria(tcb, tripulante->id_patota_asociado, TCB);
		pthread_mutex_unlock(&mutex_tocar_memoria);
		log_info(logger, "Se subio a memoria el tripulante");

	}
	else{

	}

}


void preparar_memoria_para_esquema_de_segmentacion() {
	lista_segmentos_libres = list_create();
	lista_tablas_segmentos = list_create();
	tabla_asociadores_segmentos = list_create();

	pthread_mutex_init(&mutex_tablas_segmentos, NULL);

	t_segmento* segmento = malloc(sizeof(t_segmento));

	segmento->inicio = 0;
	segmento->nro_segmento = 0;
	segmento->tamanio = tamanio_memoria;

	list_add(lista_segmentos_libres, segmento);
	log_info(logger, "Creamos las estructuras necesarias para segmentacion");

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

void escribir_en_memoria(void* informacion, uint32_t patota_asociada, e_tipo_dato tipo_dato){
	t_buffer* buffer;
	switch(tipo_dato){
		case TAREAS:
			buffer = serializar_memoria_tareas((char*) informacion);
		break;
		case PCB:
			buffer = serializar_memoria_pcb((t_pcb*) informacion);
		break;
		case TCB:
			buffer = serializar_memoria_tcb((t_tcb*) informacion);
		break;
	}

	if(son_iguales(esquema_memoria, "SEGMENTACION")) {
		escribir_en_memoria_segmentacion(buffer, patota_asociada, tipo_dato);
	}
	else{

	}
}

void modificar_memoria(void* informacion, uint32_t id_patota, e_tipo_dato tipo_dato){
	t_buffer* buffer;
	switch(tipo_dato){
		case TAREAS:
			buffer = serializar_memoria_tareas((char*) informacion);
		break;
		case PCB:
			buffer = serializar_memoria_pcb((t_pcb*) informacion);
		break;
		case TCB:
			buffer = serializar_memoria_tcb((t_tcb*) informacion);
		break;
	}

	if(son_iguales(esquema_memoria, "SEGMENTACION")) {
		modificar_memoria_segmentacion(buffer, id_patota, tipo_dato);
	}
	else{

	}
}

void mover_tripulante_memoria(t_mover_hacia* mover_hacia){
	t_tcb* tcb_viejo = deserializar_memoria_tcb(leer_memoria(mover_hacia->id_tripulante, mover_hacia->id_patota_asociado, TCB));
	log_info(logger, "RAM :: TCB obtenido:\nTripulante %i", tcb_viejo->tid);

	if(son_iguales(esquema_memoria, "SEGMENTACION")) {
		tcb_viejo->posicion->pos_x = mover_hacia->posicion_destino->pos_x;
		tcb_viejo->posicion->pos_y = mover_hacia->posicion_destino->pos_y;
	}

	modificar_memoria(tcb_viejo, mover_hacia->id_patota_asociado, TCB);

}

uint32_t obtener_direccion_tarea()

t_tarea* obtener_tarea_memoria(t_tripulante* tripulante){
	t_segmento* segmento = buscar_segmento_id(tripulante->id, tripulante->id_patota_asociado, TCB);
	t_tcb* tcb = deserializar_memoria_tcb(leer_memoria_segmentacion(segmento));
	void* stream_pcb = malloc(TAMANIO_PCB);
	memcpy(stream_pcb, memoria + tcb->puntero_pcb, TAMANIO_PCB);

	t_pcb* pcb = deserializar_memoria_pcb(stream_pcb);
	char* leido = string_new();
	char c_leido;
	int i=0;
	memcpy(&c_leido, memoria + pcb->tareas, sizeof(char));
	while(c_leido != '\n'){
		string_append(&leido, &c_leido);
	}
	pcb->tareas

	char* tareas = (char*) leer_memoria(tripulante->id_patota_asociado, tripulante->id_patota_asociado, TAREAS);
	log_info(logger, "RAM :: Tareas obtenida:\n%s", tareas);

	char** lines = string_split(tareas, "\n");

	t_tabla_segmentos* tabla = dar_tabla_segmentos(tripulante->id_patota_asociado);
	if(lines[tabla->tareas_dadas]){
		pthread_mutex_lock(&tabla->mutex_obtener_tareas);
		log_info(logger, "RAM :: Tarea obtenida:\n%s", lines[tabla->tareas_dadas]);
		t_tarea* tarea = obtener_tarea_archivo(lines[tabla->tareas_dadas]);
		tabla->tareas_dadas++;
		pthread_mutex_unlock(&tabla->mutex_obtener_tareas);
		return tarea;
	}
	return NULL;
}

void* leer_memoria(uint32_t id, uint32_t id_patota, e_tipo_dato tipo_dato){
	void* informacion;
	pthread_mutex_lock(&mutex_tocar_memoria);
	if(son_iguales(esquema_memoria, "SEGMENTACION")) {
		informacion = leer_memoria_segmentacion(buscar_segmento_id(id, id_patota, tipo_dato));
	}
	else{
//		return leer_memoria_paginacion();
	}
	pthread_mutex_unlock(&mutex_tocar_memoria);
	return informacion;
}

char* dar_key_tripulante(uint32_t id_tripulante){
	char* key_tripulante = string_duplicate("TCB");
	string_append(&key_tripulante,string_itoa(id_tripulante));

	return key_tripulante;
}

t_segmento* buscar_segmento_id(uint32_t id, uint32_t id_patota, e_tipo_dato tipo_dato){
	t_tabla_segmentos* tabla = dar_tabla_segmentos(id_patota);

	switch(tipo_dato){
		case TAREAS:
			return dictionary_get(tabla->diccionario_segmentos,"TAREAS");
		break;
		case PCB:
			return dictionary_get(tabla->diccionario_segmentos,"PCB");
		break;
		case TCB:
			return dictionary_get(tabla->diccionario_segmentos, dar_key_tripulante(id));

		break;
	}

}

void* leer_memoria_segmentacion(t_segmento* segmento){
	void* dato_requerido = malloc(segmento->tamanio);

	memcpy(dato_requerido, memoria + segmento->inicio, segmento->tamanio);
	log_info(logger, "segmento %i - inicio del segmento %i y tamanio %i",segmento->nro_segmento, segmento->inicio, segmento->tamanio);

	return dato_requerido;
}

void modificar_memoria_segmentacion(t_buffer* buffer, uint32_t patota_asociada, e_tipo_dato tipo_dato){
	uint32_t id_tripulante = 0;
	if(tipo_dato == TCB)
		memcpy(&id_tripulante, buffer->stream, sizeof(uint32_t));

	t_tabla_segmentos* tabla = dar_tabla_segmentos(patota_asociada);

	t_segmento* segmento;
	switch(tipo_dato){
		case TAREAS:
			segmento = dictionary_get(tabla->diccionario_segmentos,"TAREAS");
		break;
		case PCB:
			segmento = dictionary_get(tabla->diccionario_segmentos,"PCB");
		break;
		case TCB:
			segmento = dictionary_get(tabla->diccionario_segmentos, dar_key_tripulante(id_tripulante));
		break;
	}

	subir_segmento_memoria(segmento, buffer->stream);

}

void escribir_en_memoria_segmentacion(t_buffer* buffer, uint32_t patota_asociada, e_tipo_dato tipo_dato){
	t_segmento* segmento_libre = buscar_segmento_libre(buffer->size);
	t_segmento* segmento_nuevo = dar_nuevo_segmento(segmento_libre, buffer->size);

	segmento_libre->tamanio = buffer->size;

	subir_segmento_memoria(segmento_libre, buffer->stream);
	subir_segmento_libre(segmento_nuevo);

	uint32_t id_tripulante = 0;
	if(tipo_dato == TCB)
		memcpy(&id_tripulante, buffer->stream, sizeof(uint32_t));
	subir_tabla_segmento(segmento_libre, patota_asociada, id_tripulante, tipo_dato);
}

t_tabla_segmentos* dar_tabla_segmentos(uint32_t id_patota){
	bool el_que_quiero(t_tabla_segmentos* tabla){
		return tabla->id_patota_asociada == id_patota;
	}

	return (t_tabla_segmentos*) list_find(lista_tablas_segmentos, el_que_quiero);
}

void subir_tabla_segmento(t_segmento* segmento, uint32_t id_patota, uint32_t id_tripulante, e_tipo_dato tipo_dato){
	t_tabla_segmentos* tabla_segmentos = dar_tabla_segmentos(id_patota);

	switch(tipo_dato){
		case TAREAS:
			dictionary_put(tabla_segmentos->diccionario_segmentos,"TAREAS", segmento);
		break;
		case PCB:
			dictionary_put(tabla_segmentos->diccionario_segmentos,"PCB", segmento);
		break;
		case TCB:
			dictionary_put(tabla_segmentos->diccionario_segmentos, dar_key_tripulante(id_tripulante), segmento);
		break;
	}

}

t_segmento* dar_nuevo_segmento(t_segmento* segmento, uint32_t size){
	t_segmento* segmento_nuevo = malloc(sizeof(t_segmento));

	segmento_nuevo->inicio = segmento->inicio + size;
	segmento_nuevo->tamanio = segmento->tamanio - size;
	segmento_nuevo->nro_segmento = 0; // TODO ESTO NO ES ASI, ARREGLAR

	return segmento_nuevo;
}

void subir_segmento_memoria(t_segmento* segmento, void* stream){
	memcpy(memoria + segmento->inicio, stream, segmento->tamanio);
	log_info(logger, "segmento %i - inicio del segmento %i y tamanio %i",segmento->nro_segmento, segmento->inicio, segmento->tamanio);
}

void subir_segmento_libre(t_segmento* segmento){
	bool ordenar_segmentos(t_segmento* segmento_1, t_segmento* segmento_2){
		return (segmento_1->tamanio < segmento_2->tamanio);
	}

	list_add(lista_segmentos_libres, segmento);
	list_sort(lista_segmentos_libres, ordenar_segmentos); // TODO HAY QUE ORDENAR? QUIZA SI ES BF SIRVE PA ESO Y ORDENAMOS POR TAMAÑO
}

t_segmento* buscar_segmento_libre(uint32_t espacio_requerido){
	bool esta_libre(t_segmento* segmento){ // TODO SE VA A VER SI ES FF O BF
		return (espacio_requerido <= segmento->tamanio);
	}

	return (t_segmento*) list_remove_by_condition(lista_segmentos_libres, esta_libre);
}

void escribir_en_memoria_paginacion(/*va a tener que ser una pagina*/t_pagina* pagina, bool esta_en_memoria, uint32_t idPatota, bool modificado) {
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



