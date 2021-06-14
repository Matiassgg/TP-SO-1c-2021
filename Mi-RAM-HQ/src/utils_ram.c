#include "utils_ram.h"

void arrancar_servidor(void){
	pthread_t thread;
	uint32_t socket_servidor = iniciar_servidor(logger, ip_Mi_RAM_HQ, puerto_escucha);
	while(1){
		uint32_t socket_cliente = esperar_cliente(logger,socket_servidor);

		int *socket_paraPasar = malloc(sizeof(int));
		*socket_paraPasar = socket_cliente;

		pthread_create(&thread, NULL, (void*) serve_client, socket_paraPasar);
		pthread_detach(thread);
	}
}

void serve_client(int* socket_que_nos_pasan) {
    uint32_t socket = *socket_que_nos_pasan;
	free(socket_que_nos_pasan);

	int cod_op;

	while (1) {
		int i = recv(socket, &cod_op, sizeof(op_code), MSG_WAITALL);

		if (i != 4) {
			cod_op = -1;
			liberar_conexion(&socket);
			pthread_exit(NULL);
		}
		else {
			procesar_mensaje_recibido(cod_op, socket);
		}
	}

}

void procesar_mensaje_recibido(int cod_op, int cliente_fd) {

	uint32_t buffer_size;
	t_tripulante* tripulante = malloc(sizeof(t_tripulante));
	recv(cliente_fd, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	// logguear quien se me conecto: quiza hay que agregarle a los paquetes el nombre del modulo que envió el paquete, no lo sé

	//Procesar mensajes recibidos
	switch (cod_op) {
		case ESTA_ON:
			log_info(logger, "Estamos on");
			enviar_respuesta(OK,cliente_fd);
		break;
		case INICIAR_PATOTA:
			;
			t_patota* patota = deserializar_iniciar_patota(cliente_fd);

			log_info(logger, "RAM :: Nos llego INICIAR_PATOTA de la patota %i", patota->id_patota);

			cargar_memoria_patota(patota);

			log_info(logger, "RAM :: Cargamos a memoria la patota %i y sus tareas", patota->id_patota);

//			escribir_en_memoria(pcb_nuevo);

//			guardar_memoria_pcb();

//			GUARDAR EN MEMORIA Y HACER LAS TARES CORRESPONDIENTES
//			POR AHORA SE HACE FREE CAPAZ DESPUES NO
			list_destroy(patota->posiciones);
			free(patota->path_tareas);
			free(patota);
		break;
		case INICIAR_TRIPULANTE:
			tripulante = deserializar_iniciar_tripulante(cliente_fd);

			log_info(logger, "RAM :: Nos llego INICIAR_TRIPULANTE del tripulante %i", tripulante->id);

			cargar_memoria_tripulante(tripulante);

			log_info(logger, "RAM :: Cargamos a memoria el tripulante %i de la patota %i", tripulante->id, tripulante->id_patota_asociado);
//			crear_tripulante(nivel, tripulante);	// todo


//			GUARDAR EN MEMORIA Y HACER LAS TARES CORRESPONDIENTES
//			POR AHORA SE HACE FREE CAPAZ DESPUES NO
			free(tripulante);
			break;
		case LISTAR_TRIPULANTES:

			log_info(logger, "Nos llego LISTAR_TRIPULANTES");

			// Obtener el listado de todos los tripulantes con su estado y su patota correspondiente

//			enviar_discordiador_listar_tripulantes_respuesta(tripulantes, cliente_fd); es necesario????

//			log_info(logger, "RAM :: Se enviaron los datos de los tripulantes");

		break;
		case EXPULSAR_TRIPULANTE:
			tripulante = deserializar_expulsar_tripulante(cliente_fd);

			log_info(logger, "RAM :: Nos llego EXPULSAR_TRIPULANTE para el tripulante %i", tripulante->id);

			// Eiminará un tripulante tanto de las estructuras administrativas de la memoria, como también del mapa
			// EN CASO QUE SEA NECESARIO eliminara su segmento de tareas

			// item_borrar(nivel, tripulante->id);			// todo borrar del mapa


			free(tripulante);	// por ahora no hay mucho que hacer uwu

			log_info(logger, "RAM :: Se expulso al tripulante");

		break;
		case SOLICITAR_TAREA:
			tripulante = deserializar_solicitar_tarea(cliente_fd);

			log_info(logger, "RAM :: Nos llego SOLICITAR_TAREA para el tripulante %i", tripulante->id);

			t_tarea* tarea = obtener_tarea(tripulante);

			if(tarea == NULL){
				log_warning(logger, "RAM :: Ya no hay mas tareas a esa patota");

			}

			enviar_solicitar_tarea_respuesta(tarea,cliente_fd);

			free(tripulante);

			log_info(logger, "RAM :: Se da la tarea");

		break;
		case MOVER_HACIA:
			;
			mover_hacia* mover_hacia =  deserializar_mover_hacia(cliente_fd);

			log_info(logger, "RAM :: Nos llego MOVER_HACIA del tripulante %i", mover_hacia->id_tripulante);

			free(mover_hacia);

		break;
	}
}


/*
void eliminar_patota_de_swap(uint32_t idPatota){
	bool entradas_de_la_patota(void* parametro) {
		entradaSwap* entrada = (entradaSwap *) parametro;
		return idPatota == entrada->idPatota;
	}
	t_list* marcosEnSwap = list_filter(entradas_swap, entradas_de_la_patota);

	for(int i=0; i < list_size(marcosEnSwap); i++){
		entradaSwap* entrada = list_get(marcosEnSwap,i);
		entrada->libre = true;

		borrar_de_swap(entrada->indiceMarcoSwap);
	}

}
*/


t_tarea* obtener_tarea(t_tripulante* tripulante){
//	if(list_is_empty(tareas)){
//		log_warning(logger, "No hay tareas");
//		return NULL;
//	}
//
//	t_queue* pila_tareas = list_get(tareas, tripulante->id_patota_asociado-1);
//
//	if(queue_is_empty(pila_tareas)){
//		log_warning(logger, "No hay quedan tareas para la patota");
//		return NULL;
//	}
//
//	t_tarea* tarea = queue_pop(pila_tareas);
//	return tarea;
	char* tareas = (char*) leer_memoria(tripulante->id_patota_asociado, TAREAS);
	log_info(logger, "RAM :: Tareas obtenida:\n%s", tareas);

	char** lines = string_split(tareas, "\n");
	log_info(logger, "RAM :: Tarea obtenida:\n%s", lines[0]);
	t_tarea* tarea = obtener_tarea_archivo(lines[0]);

//	void add_cofiguration(char *line) {
//		if (!string_starts_with(line, "#")) {
//			t_tarea* tarea = obtener_tarea_archivo(line);
//		}
//	}
//	string_iterate_lines(lines, add_cofiguration);
//	string_iterate_lines(lines, (void*) free);

	free(lines);

	return tarea;
}


char* obtener_tareas(t_pcb* pcb){
//	t_queue* pila_tareas = queue_create();
	char* leido = malloc(sizeof(char));
	int i=0;

	char* path = string_new();
	path = string_duplicate("/home/utnso/tp-2021-1c-LaMitad-1/discordiador/tareasPatota");

	string_append_with_format(&path, "%s.txt", string_itoa(pcb->pid));

	FILE* archivo = fopen(path, "r+");

	while(fread(leido+i,1,1,archivo)){
		i++;
		leido= (char*) realloc(leido,i+1);
	}
	leido= (char*) realloc(leido,i+1);

//	char** lines = string_split(leido, "\n");
//
//	void add_cofiguration(char *line) {
//		if (!string_starts_with(line, "#")) {
//			t_tarea* tarea = obtener_tarea_archivo(line);
////			guardar_memoria_tarea(pcb, tarea);
//			queue_push(pila_tareas, tarea);
//		}
//	}
//	string_iterate_lines(lines, add_cofiguration);
//	string_iterate_lines(lines, (void*) free);
//
//	list_add(tareas,pila_tareas);
//
//	free(lines);

	return leido;

}


t_tarea* obtener_tarea_archivo(char* tarea_string){
	t_tarea* tarea = malloc(sizeof(t_tarea));
	tarea->posicion = malloc(sizeof(t_posicion));

	char** tarea_parametros = string_n_split(tarea_string, 2, " ");
	char** parametros = string_n_split(tarea_parametros[1], 4, ";");

	tarea->tarea = obtener_nombre_tarea(tarea_parametros[0]);
	tarea->parametro = atoi(parametros[0]);
	tarea->posicion->pos_x = atoi(parametros[1]);
	tarea->posicion->pos_y = atoi(parametros[2]);
	tarea->tiempo = atoi(parametros[3]);

	return tarea;
}

e_tarea obtener_nombre_tarea(char* tarea){
	if(son_iguales(tarea,"GENERAR_OXIGENO"))
		return GENERAR_OXIGENO;
	if(son_iguales(tarea,"CONSUMIR_OXIGENO"))
		return CONSUMIR_OXIGENO;
	if(son_iguales(tarea,"GENERAR_COMIDA"))
		return GENERAR_COMIDA;
	if(son_iguales(tarea,"CONSUMIR_COMIDA"))
		return CONSUMIR_COMIDA;
	if(son_iguales(tarea,"GENERAR_BASURA"))
		return GENERAR_BASURA;
	if(son_iguales(tarea,"DESCARTAR_BASURA"))
		return DESCARTAR_BASURA;
}
/*
bool esta_en_memoria(t_pagina* pagina, uint32_t idPatota) {
	t_list* marcosEnMemoria = entradas_segun_pedido(idPatota);
	int cantidadMarcos = list_size(marcosEnMemoria);
	int offset = 0;
	bool encontrado = false;

	for (int i = 0; i < cantidadMarcos && !encontrado; i++) {
		entradaTablaMarcos* entrada = list_get(marcosEnMemoria, i);
		uint32_t cantidadLista;
		uint32_t cantidadPlato;
		char* nombrePlato = malloc(24);

		memcpy(&cantidadLista, entrada->marco + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(&cantidadPlato, entrada->marco + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(nombrePlato, entrada->marco + offset, 24);

		if (!strcmp(pagina->nombrePlato, nombrePlato)) {
			encontrado = true;
		} else {
			encontrado = false;
		}

		offset = 0;

		free(nombrePlato);
	}

	list_destroy(marcosEnMemoria);

	return encontrado;
}
*/
t_list* entradas_segun_patota(uint32_t idPatota){
	bool buscar_entradas_patota(void* parametro){
		entradaTablaMarcos* entradaEnMem = (entradaTablaMarcos*) parametro;
		return idPatota == entradaEnMem->idPatota;
	}

	t_list* entradas = list_filter(tablaDeMarcos, buscar_entradas_patota);

	return entradas;
}



