#include "memoria.h"

void iniciar_memoria() {
	// Reservar el espacio de memoria
	preparar_memoria();
	path_dump = string_duplicate("/home/utnso/tp-2021-1c-LaMitad-1");
	log_info(logger, "path_dump %s",path_dump);
	signal(SIGUSR2, dump_memoria_principal);

//	// FALTA
	// Dibujar el mapa inicial vacío
	pthread_create(&hiloReceiveMapa, NULL, (void*) iniciar_mapa_vacio, NULL);
	pthread_detach(hiloReceiveMapa);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////FUNCIONES GENERALES/////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void preparar_memoria() {
	memoria = calloc(1,tamanio_memoria);

	pthread_mutex_init(&mutex_subir_patota, NULL);
	pthread_mutex_init(&mutexTablaMarcos, NULL);
	pthread_mutex_init(&mutexEntradasSwap, NULL);
	pthread_mutex_init(&mutexMemoria, NULL);
	pthread_mutex_init(&mutexSwap, NULL);
	pthread_mutex_init(&mutexBuscarSegmento, NULL);
	pthread_mutex_init(&mutexBuscarPagina, NULL);
	pthread_mutex_init(&mutexBuscarInfoTripulante, NULL);
	pthread_mutex_init(&mutex_tocar_memoria, NULL);

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

char* dar_key_tripulante(uint32_t id_tripulante){
	char* key_tripulante = string_duplicate("TCB");
	string_append(&key_tripulante,string_itoa(id_tripulante));

	return key_tripulante;
}

void crear_tabla(uint32_t id_patota){

	if(son_iguales(esquema_memoria, "SEGMENTACION")){
		t_tabla_segmentos* tabla = malloc(sizeof(t_tabla_segmentos));
		tabla->diccionario_segmentos = dictionary_create();
		tabla->id_patota_asociada = id_patota;
		tabla->tripulantes_activos = list_create();

		pthread_mutex_lock(&mutex_tablas);
		list_add(lista_tablas_segmentos, tabla);
		pthread_mutex_unlock(&mutex_tablas);

	}else if(son_iguales(esquema_memoria, "PAGINACION")){
		t_tabla_paginas* tabla = crear_tabla_paginacion(id_patota);

		pthread_mutex_lock(&mutex_tablas);
		list_add(lista_tablas_paginas, tabla);
		pthread_mutex_unlock(&mutex_tablas);
	}
	log_info(logger, "RAM :: Se crea la tabla para la patota %d", id_patota);
}

t_pcb* crear_pcb(t_patota* patota){
	log_info(logger, "RAM :: Se crea el PCB para la patota %d", patota->id_patota);
	t_pcb* pcb = malloc(sizeof(t_pcb));

	pcb->tareas = 0;
	pcb->pid = patota->id_patota;

	return pcb;
}

t_tcb* crear_tcb(t_tripulante* tripulante){
	log_info(logger, "RAM :: Se creara el TCB para el tripualante %d de la patota %d", tripulante->id, tripulante->id_patota_asociado);
	t_tcb* tcb = malloc(sizeof(t_tcb));
	tcb->posicion = malloc(sizeof(t_posicion));

	tcb->tid = tripulante->id;
	tcb->estado = 'N'; // TODO
	tcb->prox_instruccion = obtener_direccion_tarea(tripulante->id_patota_asociado, 0);
	log_info(logger,"prox_instruccion: %i",tcb->prox_instruccion);
	tcb->posicion->pos_x = tripulante->posicion->pos_x;
	tcb->posicion->pos_y = tripulante->posicion->pos_y;
	tcb->puntero_pcb = obtener_direccion_pcb(tripulante->id_patota_asociado); // TODO CAMBIAR ESTA KK SI SE ARREGLA ESTO SE FACILITA EL DAR TAREAS

	log_info(logger, "RAM :: Se creo el TCB en NEW, para el tripulante: %d", tcb->tid);

	return tcb;
}

void cargar_memoria_patota(t_patota* patota){
	pthread_mutex_lock(&mutex_subir_patota);
	t_pcb* pcb_nuevo = crear_pcb(patota);

	char* tareas = obtener_tareas(patota);
	crear_tabla(pcb_nuevo->pid);


	if(son_iguales(esquema_memoria, "SEGMENTACION")) {

		pthread_mutex_lock(&mutex_tocar_memoria);
		escribir_en_memoria(tareas, pcb_nuevo->pid, TAREAS);
		pthread_mutex_unlock(&mutex_tocar_memoria);
		log_info(logger, "Se subio a memoria la tarea");

		t_segmento* segmento_tareas = buscar_segmento_id(pcb_nuevo->pid, pcb_nuevo->pid, TAREAS);
		pcb_nuevo->tareas = dar_direccion_logica(segmento_tareas->nro_segmento, 0);

		pthread_mutex_lock(&mutex_tocar_memoria);
		escribir_en_memoria(pcb_nuevo, pcb_nuevo->pid, PCB);
		pthread_mutex_unlock(&mutex_tocar_memoria);
		log_info(logger, "Se subio a memoria la patota");

	}
	else if(son_iguales(esquema_memoria, "PAGINACION")){

			pthread_mutex_lock(&mutex_tocar_memoria);
			escribir_en_memoria(tareas, pcb_nuevo->pid, TAREAS);
			pthread_mutex_unlock(&mutex_tocar_memoria);
			log_info(logger, "Se subio a memoria la tarea");

			pthread_mutex_lock(&mutex_tocar_memoria);
			escribir_en_memoria(pcb_nuevo, pcb_nuevo->pid, PCB);
			pthread_mutex_unlock(&mutex_tocar_memoria);
			log_info(logger, "Se subio a memoria la patota");

	}

	pthread_mutex_unlock(&mutex_subir_patota);

}

void cargar_memoria_tripulante(t_tripulante* tripulante){
	pthread_mutex_lock(&mutex_subir_patota);

	t_tcb* tcb = crear_tcb(tripulante);

	pthread_mutex_lock(&mutex_tocar_memoria);
	escribir_en_memoria(tcb, tripulante->id_patota_asociado, TCB);
	pthread_mutex_unlock(&mutex_tocar_memoria);
	log_info(logger, "Se subio a memoria el tripulante");


	pthread_mutex_unlock(&mutex_subir_patota);

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
	else if(son_iguales(esquema_memoria, "PAGINACION")){
		escribir_en_memoria_paginacion(buffer,patota_asociada,tipo_dato);
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
		modificar_memoria_paginacion(buffer, id_patota, tipo_dato);
	}
}

void mover_tripulante_memoria(t_mover_hacia* mover_hacia){
	t_tcb* tcb_viejo = deserializar_memoria_tcb(leer_memoria(mover_hacia->id_tripulante, mover_hacia->id_patota_asociado, TCB));
	log_info(logger, "RAM :: TCB obtenido:\nTripulante %i", tcb_viejo->tid);

	tcb_viejo->posicion->pos_x = mover_hacia->posicion_destino->pos_x;
	tcb_viejo->posicion->pos_y = mover_hacia->posicion_destino->pos_y;

	modificar_memoria(tcb_viejo, mover_hacia->id_patota_asociado, TCB);

}

void* leer_memoria(uint32_t id, uint32_t id_patota, e_tipo_dato tipo_dato){
	void* informacion;
	pthread_mutex_lock(&mutex_tocar_memoria);
	if(son_iguales(esquema_memoria, "SEGMENTACION")) {
		informacion = leer_memoria_segmentacion(buscar_segmento_id(id, id_patota, tipo_dato));
	}
	else{
		informacion = leer_memoria_paginacion(id, id_patota, tipo_dato);
	}
	pthread_mutex_unlock(&mutex_tocar_memoria);
	return informacion;
}

t_pagina* dar_pagina_de_tabla(t_tabla_paginas* tabla, uint32_t nro_pagina){
	bool es_pagina(t_pagina* pagina){
		return pagina->numeroPagina == nro_pagina;
	}

	return list_find(tabla->paginas,es_pagina);
}

t_tcb* obtener_tripulante_memoria(uint32_t id_tripulante, uint32_t id_patota_asociada){
	return (t_tcb*) deserializar_memoria_tcb(leer_memoria(id_tripulante, id_patota_asociada, TCB));
}

t_tarea* obtener_tarea_memoria(t_tripulante* tripulante){

	if(son_iguales(esquema_memoria, "SEGMENTACION")) {
		t_segmento* segmento = buscar_segmento_id(tripulante->id, tripulante->id_patota_asociado, TCB);
		t_tcb* tcb = deserializar_memoria_tcb(leer_memoria_segmentacion(segmento));

		t_segmento* segmento_tareas = buscar_segmento_id(tripulante->id_patota_asociado, tripulante->id_patota_asociado, TAREAS);
		//	char* tareas = (char*) leer_memoria_segmentacion(segmento_tareas);
		//	log_info(logger,"tareas: %s",tareas);

		char* tarea_string = string_new();
		char* c_leido = calloc(2,sizeof(char));
		uint32_t new_offset=0;
		uint32_t offset = dar_offset_direccion_logica(tcb->prox_instruccion);
		log_info(logger,"offset: %i",offset);

		if(offset >= segmento_tareas->tamanio)
			return NULL;

		uint32_t inicio_tarea = segmento_tareas->inicio + offset;
		log_info(logger,"inicio_tarea: %i",inicio_tarea);
		do{
			memcpy(c_leido, memoria + inicio_tarea + new_offset, sizeof(char));
			new_offset++;
			string_append(&tarea_string, c_leido);
		}while((c_leido[0] != '\n') && ((offset + new_offset) <= segmento_tareas->tamanio));
		log_info(logger,"tarea_string: %s",tarea_string);
		log_info(logger,"new_offset: %i",new_offset);

		tcb->prox_instruccion = dar_direccion_logica(segmento_tareas->nro_segmento,new_offset+offset);
		modificar_memoria_segmentacion(serializar_memoria_tcb(tcb),tripulante->id_patota_asociado,TCB);
		t_tarea* tarea = obtener_tarea_archivo(tarea_string);

		return tarea;
	}else if(son_iguales(esquema_memoria, "PAGINACION")) {
		t_tcb* tcb  = deserializar_memoria_tcb(leer_memoria(tripulante->id, tripulante->id_patota_asociado, TCB));

		log_info(logger,"prox_instruccion: %i",tcb->prox_instruccion);
		uint32_t offset = dar_offset_direccion_logica(tcb->prox_instruccion);
		log_info(logger,"offset: %d",offset);

		if(offset >= tamanio_pagina)
			return NULL;
		uint32_t nro_pagina = dar_numero_particion_direccion_logica(tcb->prox_instruccion);

		t_tabla_paginas* tabla = dar_tabla_paginas(tripulante->id_patota_asociado);
		t_pagina* pagina = dar_pagina_de_tabla(tabla, nro_pagina);
		t_asociador_pagina* asociador = dar_asociador_pagina(pagina, tripulante->id_patota_asociado, TAREAS);

		if(offset >= asociador->tamanio)
			return NULL;
		char* tarea_string = string_new();
		char* c_leido = calloc(2,sizeof(char));
		uint32_t new_offset=0;

		uint32_t inicio_tarea = pagina->marco->inicioMemoria + offset;
		log_info(logger,"inicio_tarea: %i",inicio_tarea);
		do{
			memcpy(c_leido, memoria + inicio_tarea + new_offset, sizeof(char));
			new_offset++;
			string_append(&tarea_string, c_leido);
		}while((c_leido[0] != '\n') && ((offset + new_offset) < tamanio_pagina) && ((offset + new_offset) < asociador->tamanio));
		log_info(logger,"tarea_string: %s",tarea_string);
		log_info(logger,"new_offset: %i",new_offset);

		nro_pagina = pagina->numeroPagina;
		uint32_t offset_logico = new_offset+offset;

		if((c_leido[0] != '\n') && (offset + new_offset) >= tamanio_pagina){
			log_info(logger,"Se entro al if 2");
			t_list* paginas = obtener_paginas_asignadas(tabla, tripulante->id_patota_asociado, TAREAS);
			if(list_size(paginas) > 1){
				bool aux = false;
				bool dar_proxima_pagina(t_pagina* pagina_A){
					if(pagina_A->numeroPagina == pagina->numeroPagina){
						aux = true;
						return false;
					}
					return aux;
				}
				t_pagina* pagina_aux = list_find(paginas, dar_proxima_pagina);
				t_asociador_pagina* asociador_2 = dar_asociador_pagina(pagina_aux, tripulante->id_patota_asociado, TAREAS);
				offset = asociador_2->inicio;

				new_offset=0;
				log_info(logger,"offset: %d",offset);

				if(offset >= tamanio_pagina)
					return NULL;

				inicio_tarea = pagina_aux->marco->inicioMemoria + offset;
				log_info(logger,"inicio_tarea: %i",inicio_tarea);

				while((c_leido[0] != '\n') && ((offset + new_offset) < tamanio_pagina) && ((offset + new_offset) < asociador_2->tamanio)){
					memcpy(c_leido, memoria + inicio_tarea + new_offset, sizeof(char));
					new_offset++;
					string_append(&tarea_string, c_leido);
				}

				nro_pagina = pagina_aux->numeroPagina;
				offset_logico = new_offset+offset;

				log_info(logger,"tarea_string: %s",tarea_string);
				log_info(logger,"nro_pagina: %i",nro_pagina);

			}
		}

		tcb->prox_instruccion = dar_direccion_logica(nro_pagina,offset_logico);
		modificar_memoria_paginacion(serializar_memoria_tcb(tcb),tripulante->id_patota_asociado,TCB);
		t_tarea* tarea = obtener_tarea_archivo(tarea_string);

		return tarea;
	}
	return NULL;
}

void sacar_de_memoria(uint32_t id, uint32_t patota_asociada, e_tipo_dato tipo_dato){

	if(son_iguales(esquema_memoria, "SEGMENTACION")) {
		liberar_segmento( sacar_de_tabla_segmentacion(id, patota_asociada, tipo_dato) );
	}
	else{
		t_tabla_paginas* tabla = dar_tabla_paginas(patota_asociada);
		if(tipo_dato == TCB){
			bool es_id(uint32_t id_tripulante){
				return id_tripulante == id;
			}
			list_remove_by_condition(tabla->tripulantes_activos,es_id);
			log_info(logger, "Se saco de la lista de tripulantes activos al tripulante %i", id);
		}
		t_list* paginas = obtener_paginas_asignadas(tabla, id, tipo_dato);

		for(int i=0; i<list_size(paginas);i++){
			t_pagina* pagina = list_get(paginas,i);
			t_asociador_pagina* asociador = quitar_asociador_pagina(pagina, id, tipo_dato);
			pagina->espacio_libre += asociador->tamanio;

			log_info(logger, "Se saco desde %i %i bytes dejando %i libres", pagina->marco->inicioMemoria + asociador->inicio, asociador->tamanio, pagina->espacio_libre);
			free(asociador);
		}
	}
}

void expulsar_tripulante(t_tripulante* tripulante){
	sacar_de_memoria(tripulante->id, tripulante->id_patota_asociado, TCB);

	eliminar_tripulante(tripulante->id);
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

FILE* crear_archivo_dump(){
	char* path_dump_actual = string_duplicate(path_dump);
	char* timestamp = temporal_get_string_time("%d%m%y%H%M%S");
	char* timestamp_2 = temporal_get_string_time("%d/%m/%y %H:%M:%S");
	string_append_with_format(&path_dump_actual, "/Dump_%s.dmp",timestamp);
	log_info(logger, "path_dump_actual %s",path_dump_actual);
	FILE* dump = fopen(path_dump_actual, "w+");

	char* stream_dump = string_duplicate("--------------------------------------------------------------------------\n");
	string_append_with_format(&stream_dump, "Dump: %s\n",timestamp_2);

	txt_write_in_file(dump, stream_dump);
	free(timestamp);
	free(timestamp_2);
	free(stream_dump);
//	string_append(&stream_dump, "--------------------------------------------------------------------------");
//	fclose(dump);
	return dump;
}

/* SUPONGO QUE EL DUMP DE ALGUNA MANERA VA A TENER QUE RECIBIR PATOTAS PARA MOSTRAR. SI EL
 * ESQUEMA ES SEGMENTACIÓN, ESA PATOTA TENDRÁ UNA TABLA DE SEGMENTOS ASIGNADA, Y DE AHÍ VAMOS
 * A LISTAR TODOS LOS RESPECTIVOS SEGMENTOS CON SU LUGAR EN MEMORIA Y TAMAÑO. SI ES PAGINACIÓN
 * TAMBIEN MOSTRARA TODOS LOS FRAMES CON SU RESPECTIVO ESTADO, Y EL ID DE LA PATOTA QUE TIENE
 * ASIGNADO ESE FRAME JUNTO CON EL NUMERO DE PAGINA.
 *
 */


void agregar_a_dump(char* stream){
	FILE* dump = fopen(path_dump, "r+");

	fseek(dump,TAM_GUIONES,SEEK_END);
	fwrite(stream,string_length(stream)+1,1,dump);

	fclose(dump);
}

char* agregar_segmento_dump(uint32_t pid, t_segmento* segmento, uint32_t nro_segmento){
	char* stream_aux = string_duplicate("Proceso:");
	string_append_with_format(&stream_aux, " %-3i Segmento: %-3i Inicio: 0x%04X	Tam: %ib\n", pid, nro_segmento, segmento->inicio, segmento->tamanio);

	return stream_aux;
}

char* dar_string_estado(bool estado){
	if(estado)
		return "Ocupado";
	return "Libre";
}

char* agregar_pagina_dump(uint32_t pid, t_pagina* pagina){
	char* stream_aux = string_duplicate("Marco:");
	if(pagina->marco->bitUso)
		string_append_with_format(&stream_aux, " %-3i Estado: %-3i Proceso: %-3i	Pagina: %-3i\n", pagina->marco->numeroMarco,dar_string_estado(pagina->marco->bitUso), pid, pagina->numeroPagina);
	else
		string_append_with_format(&stream_aux, " %-3i Estado: %-3i Proceso: %-3s	Pagina: %-3s\n", pagina->marco->numeroMarco,dar_string_estado(pagina->marco->bitUso), "-", "-");

	return stream_aux;
}

void dump_memoria_principal(){
	log_info(logger, "Se realiza un DUMP de la memoria principal");
	FILE* dump = crear_archivo_dump();
	if(dump == NULL){
		log_error(logger, "No se pudo inicializar el dump de forma correcta");
		return;
	}

	log_info(logger, "Se inicializo el dump de forma correcta");
	char* stream_dump = string_new();
	if(son_iguales(esquema_memoria, "SEGMENTACION")) {
		for(int i=0;i<list_size(lista_tablas_segmentos);i++){
			//TODO HAY QUE ORDENAR LA LISTA DE TABLAS DE SEGMENTOS POR NRO DE PID
			t_tabla_segmentos* tabla_segmentos = list_get(lista_tablas_segmentos,i);
			uint32_t id_patota_asociada = tabla_segmentos->id_patota_asociada;
			t_dictionary* diccionario_tabla = tabla_segmentos->diccionario_segmentos;

			t_segmento* segmento_tareas = dictionary_get(diccionario_tabla, "TAREAS");
			t_segmento* segmento_pcb = dictionary_get(diccionario_tabla, "PCB");

			string_append(&stream_dump,agregar_segmento_dump(id_patota_asociada, segmento_tareas,0));
			string_append(&stream_dump,agregar_segmento_dump(id_patota_asociada, segmento_pcb,1));

			for(int j=0; j<list_size(tabla_segmentos->tripulantes_activos); j++){
				int k = 2;
				t_segmento* segmento_tripulante = dictionary_get(diccionario_tabla, dar_key_tripulante(list_get(tabla_segmentos->tripulantes_activos,j)));
				string_append(&stream_dump,agregar_segmento_dump(id_patota_asociada, segmento_tripulante,k));
				k++;
			}
		}
	}
	else if(son_iguales(esquema_memoria,"PAGINACION")){
		for(int i=0;i<list_size(tablaDeMarcos);i++){
			t_marco* marco = list_get(tablaDeMarcos,i);
			t_pagina* pagina_asociada = obtenerPaginaAsociada(marco);

			bool es_tabla(t_tabla_paginas* tabla){
				bool esta_marco(t_pagina* pagina){
					if(pagina->marco)
						return pagina->marco->numeroMarco == marco->numeroMarco;
					return false;
				}
				return list_any_satisfy(tabla->paginas, esta_marco);
			}
			t_tabla_paginas* tabla_paginas = list_find(lista_tablas_paginas, es_tabla);

			string_append(&stream_dump,agregar_pagina_dump(tabla_paginas->id_patota_asociada, pagina_asociada));
		}
	}
	string_append(&stream_dump, "--------------------------------------------------------------------------");
	txt_write_in_file(dump, stream_dump);
	fclose(dump);

	free(stream_dump);

}

/*SUPONGO QUE ACÁ RECIBE EL TRIPULANTE CON EL ESTADO YA CAMBIADO, Y SE DEBE IR A BUSCAR ESE MISMO
 * TRIPULANTE A MEMORIA, TRAERLO, Y COPIARLE EL ESTADO DEL QUE NOS PASARON AL QUE ESTABA EN MEMORIA
 */

//void modificar_memoria_estado_tripulante(t_tripulante* tripulante){
//
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////SEGMENTACIÓN/////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ordenar_segmentos(){
	pthread_mutex_lock(&mutex_tablas);
	uint32_t inicio_nuevo = 0;
	for(int i=0; i<list_size(lista_tablas_segmentos); i++){
		t_tabla_segmentos* tabla = (t_tabla_segmentos*) list_get(lista_tablas_segmentos, i);

		t_segmento* segmento_pcb = dictionary_get(tabla->diccionario_segmentos, "PCB");
		segmento_pcb->inicio = inicio_nuevo;
		inicio_nuevo += segmento_pcb->tamanio;

		t_segmento* segmento_tareas = dictionary_get(tabla->diccionario_segmentos, "TAREAS");
		segmento_tareas->inicio = inicio_nuevo;
		inicio_nuevo += segmento_tareas->tamanio;

		for(int j=0; j<list_size(tabla->tripulantes_activos); j++){
			int id_tripulante = list_get(tabla->tripulantes_activos,j);
			t_segmento* segmento_tripulante = dictionary_get(tabla->diccionario_segmentos, dar_key_tripulante(id_tripulante));
			segmento_tripulante->inicio = inicio_nuevo;
			inicio_nuevo += segmento_tripulante->tamanio;
		}
	}
	log_info(logger, "COMPACTACION :: Ya se compactaron todos los segmentos y terminan en %i dando un tamaño libre de %i", inicio_nuevo, tamanio_memoria - inicio_nuevo);

	list_clean_and_destroy_elements(lista_segmentos_libres, free);
	log_info(logger, "COMPACTACION :: Se eliminaron los viejos segmentos libres (list_size=%i)", list_size(lista_segmentos_libres));

	t_segmento* segmento_nuevo = malloc(sizeof(t_segmento));
	segmento_nuevo->inicio = inicio_nuevo;
	segmento_nuevo->nro_segmento = 0;
	segmento_nuevo->tamanio = tamanio_memoria - inicio_nuevo;
	list_add(lista_segmentos_libres, segmento_nuevo);
	log_info(logger, "COMPACTACION :: Se creo el nuevo segmento libre");

	pthread_mutex_unlock(&mutex_tablas);
}

void compactar_memoria(int signum) {
	log_info(logger, "COMPACTACION :: Recibimos la señal para realizar la compactacion\n");
	pthread_mutex_lock(&mutex_tocar_memoria);
	ordenar_segmentos();

	pthread_mutex_unlock(&mutex_tocar_memoria);

}

void preparar_memoria_para_esquema_de_segmentacion() {
	lista_segmentos_libres = list_create();
	lista_tablas_segmentos = list_create();
	tabla_asociadores_segmentos = list_create();

	pthread_mutex_init(&mutex_tablas, NULL);

	t_segmento* segmento = malloc(sizeof(t_segmento));

	segmento->inicio = 0;
	segmento->nro_segmento = 0;
	segmento->tamanio = tamanio_memoria;

	list_add(lista_segmentos_libres, segmento);

	signal(SIGUSR1, compactar_memoria);

	log_info(logger, "Creamos las estructuras necesarias para segmentacion");

}

t_list* buscar_paginas_id(uint32_t id, uint32_t id_patota, e_tipo_dato tipo_dato){
	t_tabla_paginas* tabla = dar_tabla_paginas(id_patota);
	return obtener_paginas_asignadas(tabla, id, tipo_dato);
}

uint32_t obtener_direccion_pcb(uint32_t id_patota){
	if(son_iguales(esquema_memoria, "SEGMENTACION")){
		t_segmento* segmento = buscar_segmento_id(id_patota,id_patota,PCB);

		return dar_direccion_logica(segmento->nro_segmento,0);
	}
	else if(son_iguales(esquema_memoria, "PAGINACION")){
		t_list* paginas = buscar_paginas_id(id_patota, id_patota, PCB);
		t_pagina* pagina = list_get(paginas, 0);
		t_asociador_pagina* asociador = dar_asociador_pagina(pagina,id_patota,PCB);

		return dar_direccion_logica(pagina->numeroPagina,asociador->inicio);
	}
}


uint32_t obtener_direccion_tarea(uint32_t id_patota, uint32_t offset){
	if(son_iguales(esquema_memoria, "SEGMENTACION")){
		t_segmento* segmento = buscar_segmento_id(id_patota,id_patota,TAREAS);
		log_info(logger, "segmento->inicio = %i", segmento->inicio);

		return dar_direccion_logica(segmento->nro_segmento,0);
	}
	else if(son_iguales(esquema_memoria, "PAGINACION")){
		t_list* paginas = buscar_paginas_id(id_patota, id_patota, TAREAS);
		t_pagina* pagina = list_get(paginas, 0);
		t_asociador_pagina* asociador = dar_asociador_pagina(pagina,id_patota,TAREAS);

		return dar_direccion_logica(pagina->numeroPagina,asociador->inicio);
	}

}

uint32_t dar_direccion_logica(uint32_t nro_segmento, uint32_t offset){
	return nro_segmento*TAMANIO_OFFSET + offset;
}

uint32_t dar_numero_particion_direccion_logica(uint32_t dir_logica){
	return dir_logica/TAMANIO_OFFSET;
}

uint32_t dar_offset_direccion_logica(uint32_t dir_logica){
	return dir_logica-(dar_numero_particion_direccion_logica(dir_logica)*TAMANIO_OFFSET);
}

t_segmento* buscar_segmento_id(uint32_t id, uint32_t id_patota, e_tipo_dato tipo_dato){
	t_tabla_segmentos* tabla = dar_tabla_segmentos(id_patota);

	switch(tipo_dato){
		case TAREAS:
			return (t_segmento*) dictionary_get(tabla->diccionario_segmentos,"TAREAS");
		break;
		case PCB:
			return (t_segmento*) dictionary_get(tabla->diccionario_segmentos,"PCB");
		break;
		case TCB:
			return (t_segmento*) dictionary_get(tabla->diccionario_segmentos, dar_key_tripulante(id));
		break;
	}
	log_error(logger, "No se encontro segmento");
	return NULL;

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
	log_info(logger, "segmento->inicio = %i", segmento->inicio);

	switch(tipo_dato){
		case TAREAS:
			dictionary_put(tabla_segmentos->diccionario_segmentos,"TAREAS", segmento);
		break;
		case PCB:
			dictionary_put(tabla_segmentos->diccionario_segmentos,"PCB", segmento);
		break;
		case TCB:
			dictionary_put(tabla_segmentos->diccionario_segmentos, dar_key_tripulante(id_tripulante), segmento);
			list_add(tabla_segmentos->tripulantes_activos, id_tripulante);
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
	if(son_iguales(criterio_seleccion, "BF"))
		list_sort(lista_segmentos_libres, ordenar_segmentos); // TODO HAY QUE ORDENAR? QUIZA SI ES BF SIRVE PA ESO Y ORDENAMOS POR TAMAÑO
}

t_segmento* buscar_segmento_libre(uint32_t espacio_requerido){
	bool esta_libre(t_segmento* segmento){ // TODO SE VA A VER SI ES FF O BF
		return (espacio_requerido <= segmento->tamanio);
	}

	return (t_segmento*) list_remove_by_condition(lista_segmentos_libres, esta_libre);
}

t_segmento* sacar_de_tabla_segmentacion(uint32_t id, uint32_t patota_asociada, e_tipo_dato tipo_dato){
	t_tabla_segmentos* tabla = dar_tabla_segmentos(patota_asociada);

	switch(tipo_dato){
		case TAREAS:
			return (t_segmento*) dictionary_remove(tabla->diccionario_segmentos,"TAREAS");
		break;
		case PCB:
			return (t_segmento*) dictionary_remove(tabla->diccionario_segmentos,"PCB");
		break;
		case TCB:
			;
			bool es_tripulante(uint32_t id_tripulante){
				return id_tripulante == id;
			}
			list_remove_by_condition(tabla->tripulantes_activos, es_tripulante);
			return (t_segmento*) dictionary_remove(tabla->diccionario_segmentos, dar_key_tripulante(id));
		break;
	}

}

void liberar_segmento(t_segmento* segmento){
	segmento->nro_segmento = 0;

	subir_segmento_libre(segmento);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////PAGINACIÓN/////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void preparar_memoria_para_esquema_de_paginacion() {
	cantidad_de_marcos = 0;
	cantidad_de_marcos_swap=0;
	tablaDeMarcos = list_create();
	marcos_swap = list_create();
	lista_tablas_paginas = list_create();

	crear_archivo_swap();

//	if((espacio_swap = fopen(path_swap,"wb+")) == NULL){
//		log_error(logger, "No se pudo crear el espacio de SWAP");
////		return; taria bien?
//	}


	for(int offset = 0; offset < tamanio_memoria -1; offset += tamanio_pagina){
		// Cargar estructuras administrativass
//		log_info(logger, "Desplazamiento: %d", offset);

		t_marco* nuevoMarco = malloc(sizeof(t_marco));
		nuevoMarco->inicioMemoria = offset;
		nuevoMarco->numeroMarco = cantidad_de_marcos;
		nuevoMarco->bitUso = false;
		nuevoMarco->timeStamp = NULL;

		list_add(tablaDeMarcos, nuevoMarco);
//		log_info(logger, "Marco numero: %d", nuevoMarco->numeroMarco);
		cantidad_de_marcos++;
	}

	// Aca viene la parte de swap
	for(int offset = 0; offset < tamanio_swap -1; offset += tamanio_pagina){

		t_marco_en_swap* nuevoMarco = malloc(sizeof(t_marco_en_swap));
		nuevoMarco->inicioSwap = offset;
		nuevoMarco->numeroMarcoSwap = cantidad_de_marcos_swap;
		nuevoMarco->idPatota = -1;
		nuevoMarco->bitUso = false;
		nuevoMarco->nroPagina = -1;

		list_add(marcos_swap, nuevoMarco);
		cantidad_de_marcos_swap++;
	}

	log_info(logger, "Cantidad de Marcos Libres: %d", cantidad_de_marcos);
	log_info(logger, "Cantidad de Marcos en SWAP: %d",cantidad_de_marcos_swap);

}

uint32_t cantidad_paginas_pedidas() {
	div_t aux = div(TAMANIO_INICIAL_TABLA_PAGINACION, tamanio_pagina);
		if (aux.rem == 0) {
			return aux.quot;
		} else {
			return aux.quot + 1;
		}
}

t_pagina* crear_pagina() {
	t_pagina* pagina = malloc(sizeof(t_pagina));
	pagina->bit_presencia = 0;
	pagina->marco = NULL;
	pagina->diccionario_pagina = dictionary_create();
	return pagina;
}

t_tabla_paginas* crear_tabla_paginacion(uint32_t id_patota) {
	t_tabla_paginas* tabla = malloc(sizeof(t_tabla_paginas));
	tabla->paginas = list_create();
	tabla->id_patota_asociada = id_patota;
	tabla->tripulantes_activos = list_create();

	int cantidadDePaginas = cantidad_paginas_pedidas();
	t_pagina *pagina;

	for (int i = 0; i < cantidadDePaginas; i++) {
		pagina = crear_pagina();
		pagina->numeroPagina=i;
		pagina->espacio_libre=tamanio_pagina;
		asignar_marco(pagina);
		list_add(tabla->paginas, pagina);
	}
	return tabla;
}

void asignar_marco(t_pagina* pagina) {
	pthread_mutex_lock(&mutex_marcos);
	t_marco *marco = buscar_marco_libre();
	if(!marco){
		log_error(logger,"No se puede asignar un marco, tanto memoria como SWAP está completa.");
	} else {
		pagina->marco = marco;
		pagina->marco->bitUso = true;
		pagina->bit_presencia = true; //TODO seria true? --> Si el marco asignado es de SWAP se supone que no,
									  // 					 si es de memoria si.
	}

	pthread_mutex_unlock(&mutex_marcos);
}

t_marco* buscar_marco_libre(){

	bool marco_esta_libre(t_marco* marco){
			return !marco->bitUso;
	}

	//TODO acá hay que corroborar que si no hay marcos libres en MEMORIA, SI HAYA en SWAP? Porque
	//si se quiere escribir en una página, y no tengo marcos libres ni en swap ni en memoria, tengo que
	//denegar la solicitud
	if (!list_any_satisfy(tablaDeMarcos,marco_esta_libre)) {
		log_info(logger, "No hay marcos libres. Seleccionando victima.");

		pthread_t hilo;

		pthread_mutex_lock(&mutexVictima);
		pthread_create(&hilo, NULL, (void*) seleccionar_victima, NULL);
		pthread_join(hilo, 0);
		pthread_mutex_unlock(&mutexVictima);
	}

	return  list_find(tablaDeMarcos, marco_esta_libre);
}

//bool hay_marcos_libres(void){
//	bool marco_esta_libre(t_marco* marco){
//		return !marco->bitUso;
//	}
//
//	t_list* marcos_libres = list_filter(tablaDeMarcos, marco_esta_libre);
//
//	int tamanio = list_size(marcos_libres);
//
//	list_destroy(marcos_libres);
//
//	return list_any_satisfy(tablaDeMarcos,marco_esta_libre);
//}


//t_marco* buscar_marco(uint32_t marco){
//	bool el_que_quiero(void* parametro){
//		return ((t_marco*)parametro)->numeroMarco == marco;
//	}
//
//	return list_find(tablaDeMarcos, el_que_quiero);
//}

void asignar_asociador_pagina(t_pagina* pagina, uint32_t id_tripulante, t_asociador_pagina* asociador,  e_tipo_dato tipo_dato){
	switch(tipo_dato){
		case TAREAS:
			dictionary_put(pagina->diccionario_pagina,"TAREAS", asociador);
		break;
		case PCB:
			dictionary_put(pagina->diccionario_pagina,"PCB", asociador);
		break;
		case TCB:
			dictionary_put(pagina->diccionario_pagina, dar_key_tripulante(id_tripulante), asociador);
		break;
	}
}

void escribir_en_memoria_paginacion(t_buffer* buffer, uint32_t id_patota_asociada, e_tipo_dato tipo_dato){ // TODO La pagina no la deberia buscara aca?
	t_tabla_paginas* tabla = dar_tabla_paginas(id_patota_asociada);
	uint32_t tamanio_restante = buffer->size;
	uint32_t id_tripulante = 0;
	if(tipo_dato == TCB){
		memcpy(&id_tripulante, buffer->stream, sizeof(uint32_t));
		list_add(tabla->tripulantes_activos, id_tripulante);
	}

	while(tamanio_restante > 0){
		t_pagina* pagina_libre = obtener_pagina_libre_tabla(tabla);
		//	TODO si la tabla esta vacia pedir marco sino ver si entra en alguna pagina
		//--> SI LA TABLA NO TIENE PAGINAS LIBRES, AHORA HICE QUE SE LE AGREGUEN MÁS
		t_marco* marco = pagina_libre->marco;

		uint32_t tamanio_subir = minimo(pagina_libre->espacio_libre, tamanio_restante);

		if (marco == NULL) {
			marco = buscar_marco_libre();
			marco->bitUso = true;

			log_info(logger, "Marco seleccionado: %d", marco->numeroMarco);

			// Agrego a lista de timestamp por marco
			if (son_iguales(algoritmo_reemplazo, "LRU")) {
				marco->timeStamp = temporal_get_string_time("%d/%m/%y %H:%M:%S");
				log_info(logger, "El timestamp del marco numero %d se ha inicializado: %s", marco->numeroMarco, marco->timeStamp);
			}

			// Agrego a lista de timestamp por marco
			if (son_iguales(algoritmo_reemplazo, "CLOCK")) {
				marco->bitUso = true;
				log_info(logger, "Los bits del marco numero %d se han inicializados: bit de uso: %d", marco->numeroMarco, marco->bitUso);
			}
		}

		/*


	// 			TODO ESTO MAS QUE EL ESCRIBIR SERIA EL MODIFICAR
		// Busco el marco de esa pagina (hay que ver como hacer para que tipo de dato buscar, PCB, tareas o TCB)
	//			t_list* marcosDeLaPatota = entradas_segun_patota(id_patota_asociada);
	//
	//			int cantidadMarcos = list_size(marcosDeLaPatota);
	//			bool encont = false;
	//			t_marco* marco;
	//
	//			for (int i = 0; i < cantidadMarcos && !encont; i++) {
	////				//REVISAR ESTO CON TOPY
	////				if(tipo_dato == PCB){
	////					marco = list_get(marcosDeLaPatota, i);
	////					int offset = 0;
	////					t_pcb* pcbEnMarco = malloc(8);
	////					memcpy(pcbEnMarco, (marco->inicioMemoria + offset), 8);
	////
	//////					if (!strcmp(pagina->tipo_dato, pcbEnMarco)) //CAMBIAR ESTO, COMPARA UN ENUM CON UN t_pcb
	////						encont = true;
	////					free(pcbEnMarco);
	////				}
	////				else if(tipo_dato == TCB){
	////
	////				}
	////				else if(tipo_dato == TAREAS){
	////
	////				}
	//			}

	 */

		uint32_t offset = tamanio_pagina - pagina_libre->espacio_libre;
		uint32_t offset_stream = buffer->size - tamanio_restante;

		memcpy(memoria + marco->inicioMemoria + offset, buffer->stream + offset_stream, tamanio_subir);

		t_asociador_pagina* asociador = malloc(sizeof(t_asociador_pagina));
		asociador->inicio = offset;
		asociador->tamanio = tamanio_subir;
		log_info(logger, "asociador con inicio en %i y tamanio %i", asociador->inicio, asociador->tamanio);
		asignar_asociador_pagina(pagina_libre, id_tripulante, asociador, tipo_dato);

		if (son_iguales(algoritmo_reemplazo, "LRU")) {
			marco->timeStamp = temporal_get_string_time("%d/%m/%y %H:%M:%S");
		}
		if (son_iguales(algoritmo_reemplazo, "CLOCK_ME")) {
			marco->bitUso = true;
		}

		pagina_libre->espacio_libre -= tamanio_subir;
		tamanio_restante -= tamanio_subir;

		log_info(logger, "Se subio %i en la pagina %i que contiene el marco %i", tamanio_subir, pagina_libre->numeroPagina, pagina_libre->marco->numeroMarco);
	}

	log_info(logger, "Se subio todo");
}

t_tabla_paginas* dar_tabla_paginas(uint32_t id_patota){
	bool el_que_quiero(t_tabla_paginas* tabla){
		return tabla->id_patota_asociada == id_patota;
	}

	return (t_tabla_paginas*) list_find(lista_tablas_paginas, el_que_quiero);
}

t_pagina* obtener_pagina_libre_tabla(t_tabla_paginas* tabla){
	bool el_que_quiero(t_pagina* pagina){
		return (pagina->espacio_libre > 0);
	}

	if(!list_any_satisfy(tabla->paginas,el_que_quiero)){
		agregar_paginas(tabla,1);
	}

	return (t_pagina*) list_find(tabla->paginas, el_que_quiero);
}

void agregar_paginas(t_tabla_paginas* tabla, uint32_t cantidad){
	t_pagina *pagina;
	int indicePagina= list_size(tabla->paginas);
		for (int i = 0; i < cantidad; i++) {
			pagina = crear_pagina();
			pagina->numeroPagina=indicePagina;
			pagina->espacio_libre=tamanio_pagina;
			asignar_marco(pagina);
			list_add(tabla->paginas, pagina);
		}
}

t_list* obtener_paginas_asignadas(t_tabla_paginas* tabla, uint32_t id_tripulante, e_tipo_dato tipo_dato){
	char* key = string_new();
	switch(tipo_dato){
		case TAREAS:
			key = string_duplicate("TAREAS");
		break;
		case PCB:
			key = string_duplicate("PCB");
		break;
		case TCB:
			key = dar_key_tripulante(id_tripulante);
		break;
	}

	bool contiene_dato(t_pagina* pagina){
		return dictionary_has_key(pagina->diccionario_pagina, key);
	}

	return list_filter(tabla->paginas, contiene_dato);
}

t_asociador_pagina* quitar_asociador_pagina(t_pagina* pagina, uint32_t id_tripulante, e_tipo_dato tipo_dato){
	switch(tipo_dato){
		case TAREAS:
			log_info(logger, "Se busca asociador de TAREAS");
			return (t_asociador_pagina*) dictionary_remove(pagina->diccionario_pagina,"TAREAS");
		break;
		case PCB:
			log_info(logger, "Se busca asociador de PCB");
			return (t_asociador_pagina*) dictionary_remove(pagina->diccionario_pagina,"PCB");
		break;
		case TCB:
			;
			char* key_tcb = dar_key_tripulante(id_tripulante);
			log_info(logger, "Se busca asociador de %s para el tripulante %i en la pagina %i", key_tcb, id_tripulante, pagina->numeroPagina);
			return (t_asociador_pagina*) dictionary_remove(pagina->diccionario_pagina, key_tcb);
	}
}

t_asociador_pagina* dar_asociador_pagina(t_pagina* pagina, uint32_t id_tripulante, e_tipo_dato tipo_dato){
	switch(tipo_dato){
		case TAREAS:
			log_info(logger, "Se busca asociador de TAREAS");
			return (t_asociador_pagina*) dictionary_get(pagina->diccionario_pagina,"TAREAS");
		break;
		case PCB:
			log_info(logger, "Se busca asociador de PCB");
			return (t_asociador_pagina*) dictionary_get(pagina->diccionario_pagina,"PCB");
		break;
		case TCB:
			;
			char* key_tcb = dar_key_tripulante(id_tripulante);
			log_info(logger, "Se busca asociador de %s para el tripulante %i en la pagina %i", key_tcb, id_tripulante, pagina->numeroPagina);
			return (t_asociador_pagina*) dictionary_get(pagina->diccionario_pagina, key_tcb);
	}
}

void modificar_memoria_paginacion(t_buffer* buffer, uint32_t patota_asociada, e_tipo_dato tipo_dato){
	uint32_t id_tripulante = 0;
	if(tipo_dato == TCB)
		memcpy(&id_tripulante, buffer->stream, sizeof(uint32_t));

	t_tabla_paginas* tabla = dar_tabla_paginas(patota_asociada);

	t_list* paginas = obtener_paginas_asignadas(tabla, id_tripulante, tipo_dato);

	uint32_t tamanio_restante = buffer->size;

	for(int i=0; i<list_size(paginas);i++){
		t_pagina* pagina = list_get(paginas,i);
		log_info(logger, "Se buscara asociador para el tripulante %i en la pagina %i", id_tripulante, pagina->numeroPagina);
		t_asociador_pagina* asociador = dar_asociador_pagina(pagina, id_tripulante, tipo_dato);
		uint32_t offset_stream = buffer->size - tamanio_restante;

		memcpy(memoria + pagina->marco->inicioMemoria + asociador->inicio, buffer->stream+ offset_stream, asociador->tamanio);
		tamanio_restante -= asociador->tamanio;
	}

	if(tamanio_restante)
		log_error(logger, "Falta info para subir");
	else
		log_info(logger, "Se actualizo correctamente");
}

void* leer_memoria_paginacion(uint32_t id, uint32_t id_patota, e_tipo_dato tipo_dato){
	t_tabla_paginas* tabla = dar_tabla_paginas(id_patota);
	t_list* paginas = obtener_paginas_asignadas(tabla, id, tipo_dato);
	void* dato_requerido;
	int tamanio_total = 0;
	uint32_t offset = 0;
	for(int i=0; i<list_size(paginas);i++){
		log_info(logger, "list_size %i", list_size(paginas));
		t_pagina* pagina = list_get(paginas,i);
		t_asociador_pagina* asociador = dar_asociador_pagina(pagina, id, tipo_dato);
		offset = tamanio_total;
		tamanio_total += asociador->tamanio;
		if(i == 0)
			dato_requerido = malloc(tamanio_total);
		else
			dato_requerido = realloc(dato_requerido,tamanio_total);

		log_info(logger, "Se va a leer desde %i %i", pagina->marco->inicioMemoria + asociador->inicio, asociador->tamanio);

		memcpy(dato_requerido + offset, memoria + pagina->marco->inicioMemoria + asociador->inicio, asociador->tamanio);
		offset += asociador->tamanio;
	}

	t_tcb* tcb = deserializar_memoria_tcb(dato_requerido);
	log_info(logger, "prox_instruccion %i - puntero_pcb %i", tcb->prox_instruccion, tcb->puntero_pcb);


	return dato_requerido;
}

t_tabla_paginas* obtenerTablaDePaginasAsociada(t_pagina* paginaEnCuestion){
	t_tabla_paginas* tablaEncontrada = NULL;

	for(int i = 0; i<list_size(lista_tablas_paginas);i++){
		t_tabla_paginas* tabla = list_get(lista_tablas_paginas,i);

		for(int j=0; j<list_size(tabla->paginas);j++){
			t_pagina* pagina = list_get(tabla->paginas,j);

			if(pagina == paginaEnCuestion){
				tablaEncontrada = tabla;
			}else
				j++;
		}
		if(tablaEncontrada==NULL)
			i++;
		else
			break;
	}

	return tablaEncontrada;
}

t_pagina* obtenerPaginaAsociada(t_marco* marco){
	t_pagina* paginaAsociada = NULL;

	for(int i=0; i<=list_size(lista_tablas_paginas);i++){
		t_tabla_paginas* tabla = list_get(lista_tablas_paginas,i);

		for(int j=0;j<=list_size(tabla->paginas);j++){
			t_pagina* pagina = list_get(tabla->paginas,j);

			if(pagina->marco==marco){
				paginaAsociada = pagina;
			}else
				j++;
		}

		if(paginaAsociada==NULL)
			i++;
		else
			break;
	}
	return paginaAsociada;
}

//////////////////////////////////////////SWAP/////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t crear_archivo_swap(){
	int fd = open(path_swap, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); // SI EXISTE ABRE EL ARCHIVO PARA LECTURA, SINO LO CREA
	struct stat statfile;
	if(fstat(fd,&statfile)==-1)
		return -1;
	llenar_archivo(fd, tamanio_swap);
	memoria_virtual = mmap(NULL,tamanio_swap,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
	close(fd);
	return 0;
}

void llenar_archivo(int fd, uint32_t tamanio){
	void* buffer = malloc(tamanio);
	char a = '\0';
	memset(buffer,a,tamanio);
	write(fd, buffer, tamanio);
	free(buffer);
}

//
//int cantidad_de_marcos_pedidos_swap(int cantidadDeMarcos) {
//	div_t aux = div(cantidadDeMarcos, 8);
//
//	if (aux.rem == 0) {
//		return aux.quot;
//	} else {
//		return aux.quot + 1;
//	}
//}

t_marco_en_swap* buscar_marco_libre_en_swap(){

	bool marco_esta_libre(t_marco_en_swap* marco){
			return !marco->bitUso;
	}

	if(list_any_satisfy(marcos_swap,marco_esta_libre)){
		return  list_find(marcos_swap, marco_esta_libre);
	}else{
		return NULL;
	}
}

void asignar_marco_en_swap_y_sacar_de_memoria(t_pagina* pagina){
	pthread_mutex_lock(&mutex_marcos);
	t_marco_en_swap* marco = buscar_marco_libre_en_swap();

	if(!marco){
		log_error(logger,"No hay marcos libres en SWAP. Se deniega la solicitud.");
	}else{
		t_tabla_paginas* tabla = obtenerTablaDePaginasAsociada(pagina);
		pagina->marco->bitUso = false;
		pagina->marco->timeStamp = NULL;
		pagina->marco = NULL;
		pagina->bit_presencia = false;

		marco->bitUso = true;
		marco->idPatota = tabla->id_patota_asociada;
		marco->nroPagina = pagina->numeroPagina;
	}
	pthread_mutex_unlock(&mutex_marcos);
}

//////////////////////////////////////////ALGORITMOS/////////////////////////////////////////////////////////////////
//////////////////////////////////////////LRU Y CLOCK/////////////////////////////////////////////////////////////////

void* convertir(char* algoritmo_nombre) {
	if (!strcmp(algoritmo_nombre, "LRU")) return (void*) seleccionar_victima_LRU;
	if (!strcmp(algoritmo_nombre, "CLOCK")) return (void*) seleccionar_victima_CLOCK;

	return NULL;
	}

void seleccionar_victima_LRU(void){
	bool mas_viejo(void* unElem, void* otroElem){
		t_marco* elem1 = unElem;
		t_marco* elem2 = otroElem;

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
	t_marco* marco_mas_viejo = list_get(tablaDeMarcos, 0);

	log_info(logger, "Victima seleccionada: %d", marco_mas_viejo->numeroMarco);

	//todo FALTA VER QUE PASA CON SWAP ACA --> JUJU ME HABIA OLVIDADO DE ESTE COMENTARIO, LO QUE PASA CON
	//SWAP ES QUE VAMOS A TENER QUE ASIGNARLE UN FRAME A LA PÁGINA QUE ESTABA EN EL MARCO EN SWAP, CALCULO
	//QUE VAMOS A USAR LA FUNCIONCITA DE ARRIBA t_pagina* obtenerPaginaAsociada(t_marco* marco)

	generar_proceso_de_pase_a_swap(marco_mas_viejo);

	// marco_mas_viejo->bitUso = false;

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

	log_info(logger, "Victima seleccionada: %d", punteroMarcoClock->numeroMarco);

	// todo Escribo en SWAP --> LO MISMO QUE ARRIBA EN LRU

	generar_proceso_de_pase_a_swap(((t_marco*)list_get(tablaDeMarcos, posicion_puntero_actual)));

	// ((t_marco*)list_get(tablaDeMarcos, posicion_puntero_actual))->bitUso = false;

	pthread_mutex_unlock(&mutexFree);
}

void generar_proceso_de_pase_a_swap(t_marco* marcoALimpiar){
	t_pagina* paginaACopiar = obtenerPaginaAsociada(marcoALimpiar);

	//ACÁ YA LIMPIO EL MARCO, Y LA PÁGINA ES LLEVADA SWAP
	asignar_marco_en_swap_y_sacar_de_memoria(paginaACopiar);

}


bool bit_uso_apagado(t_marco* marco){
	return !marco->bitUso;
}

bool bit_uso(t_marco* marco){
	return marco->bitUso;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7
