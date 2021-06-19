#include "utils_mongo.h"

void arrancar_servidor(void){
	pthread_t thread;
	uint32_t socket_servidor = iniciar_servidor(logger, ip_Mongo_Store, puerto_escucha);
	while(1){
		uint32_t socket_cliente = esperar_cliente(logger,socket_servidor);

		int *socket_para_pasar = malloc(sizeof(int));
		*socket_para_pasar = socket_cliente;

		pthread_create(&thread, NULL, (void*) serve_client, socket_para_pasar);
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
	recv(cliente_fd, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	// logguear quien se me conecto: quiza hay que agregarle a los paquetes el nombre del modulo que envió el paquete, no lo sé

	t_patota* patota = malloc(sizeof(t_patota));
	t_tarea* tarea = malloc(sizeof(t_tarea));
	t_mover_hacia* posicion = malloc(sizeof(t_mover_hacia));
	//Procesar mensajes recibidos
	switch (cod_op) {
		case ESTA_ON:
			log_info(logger, "Estamos on");
			enviar_respuesta(OK,cliente_fd);
		break;
		case INICIAR_PATOTA:
			patota = deserializar_iniciar_patota(cliente_fd);

			log_info(logger, "Nos llego INICIAR_PATOTA de la patota %i", patota->id_patota);

//			GUARDAR EN FS Y HACER LAS TARES CORRESPONDIENTES
			list_destroy(patota->posiciones);
			free(patota->path_tareas);
			free(patota);
		break;
		case COMENZAR_EJECUCION_TAREA:
			tarea = deserializar_tarea(cliente_fd);
			verificar_archivo_tarea(tarea);
			log_info(logger, "Se comenzo la ejecucion de tarea %s", tarea->tarea);

		break;
		case FINALIZAR_TAREA:
			tarea = deserializar_tarea(cliente_fd);

			log_info(logger, "Se finaliza la ejecucion de tarea %s", tarea->tarea);
		break;
		case MOVER_HACIA:
			posicion = deserializar_mover_hacia_posicion(cliente_fd);

			log_info(logger, "Se realiza movimiento desde pos (%i,%i) hacia pos(%i,%i)",
					posicion->posicion_origen->pos_x,posicion->posicion_origen->pos_y,
					posicion->posicion_destino->pos_x,posicion->posicion_destino->pos_y);
		break;

	}

}

void verificar_archivo_tarea(t_tarea* tarea){
	if(son_iguales(tarea->tarea,"GENERAR_OXIGENO") || son_iguales(tarea->tarea,"CONSUMIR_OXIGENO")){
		if(archivo_existe("Oxigeno.ims"))
			procesar_tarea(tarea);
		else
			procesar_falta_archivo(tarea,"Oxigeno.ims");
	} else if(son_iguales(tarea->tarea,"GENERAR_COMIDA") || son_iguales(tarea->tarea,"CONSUMIR_COMIDA")){
		if(archivo_existe("Comida.ims"))
			procesar_tarea(tarea);
		else
			procesar_falta_archivo(tarea,"Comida.ims");
	} else if(son_iguales(tarea->tarea,"GENERAR_BASURA") || son_iguales(tarea->tarea,"DESCARTAR_BASURA")){
		if(archivo_existe("Basura.ims"))
			procesar_tarea(tarea);
		else
			procesar_falta_archivo(tarea,"Basura.ims");
	}

}

void procesar_tarea(t_tarea* tarea){
	char* nombre_tarea = tarea->tarea;
	uint32_t cantidad = tarea->parametro; // [Consulta]¿Este parametro es la cantidad que menciona el enunciado, o es otra cosa?
	if(contiene(nombre_tarea,"GENERAR")){
		agregar_caracteres_llenado_segun_tarea(nombre_tarea,cantidad);
	}
	else if(contiene(nombre_tarea,"CONSUMIR")){
		eliminar_caracteres_llenado_segun_tarea(nombre_tarea,cantidad);
	}
	else if(son_iguales(nombre_tarea,"DESCARTAR_BASURA")){
		//eliminar_archivo("Basura.ims");
	}
}

void agregar_caracteres_llenado_segun_tarea(char* nombre_tarea, uint32_t cantidad){
	if(contiene(nombre_tarea,"OXIGENO")){
		agregar_caracteres_llenado_a_archivo("O",cantidad,"Oxigeno.ims");
	}
	else if(contiene(nombre_tarea,"COMIDA")){
		agregar_caracteres_llenado_a_archivo("C",cantidad,"Comida.ims");
	}
	else if(contiene(nombre_tarea,"BASURA")){
		agregar_caracteres_llenado_a_archivo("B",cantidad,"Basura.ims");
	}
}

void agregar_caracteres_llenado_a_archivo(char* caracter, uint32_t cantidad, char* archivo){
	//TODO
	//Agregar caracteres al archivo
}

void eliminar_caracteres_llenado_segun_tarea(char* nombre_tarea, uint32_t cantidad){
	if(contiene(nombre_tarea,"OXIGENO")){
		//quitar_caracteres_llenado_a_archivo("O",cantidad,"Oxigeno.ims");
	}
	else if(contiene(nombre_tarea,"COMIDA")){
		//quitar_caracteres_llenado_a_archivo("C",cantidad,"Comida.ims");
	}
}

void eliminar_caracteres_llenado_a_archivo(char* caracter, uint32_t cantidad, char* archivo){
	uint32_t cantidad_caracteres = cantidad_caracteres_archivo(caracter, archivo);
	if (cantidad >= cantidad_caracteres){
		//vaciar_archivo(archivo);
		log_warning(logger,"Se quisieron eliminar mas caracteres de los existentes en %s",archivo);
	}
	else{
		//TODO
		//Eliminar caracteres del archivo.
	}
}

uint32_t cantidad_caracteres_archivo(char* caracter, char* archivo){
	//TODO
	//Calcular cuantos caracteres de llenado tiene el archivo.
	return 0;
}

void procesar_falta_archivo(t_tarea* tarea,char* archivo){
	char* nombre_tarea = tarea->tarea;
	if(contiene(nombre_tarea,"GENERAR")){
		crear_archivo(archivo);
	}
	else if(contiene(nombre_tarea,"CONSUMIR")){
		informar_falta_archivo(tarea, archivo);
	}
	else if(son_iguales(nombre_tarea,"DESCARTAR_BASURA")){
		log_error(logger, "El archivo Basuras.ims no existe. Finalizando tarea DESCARTAR_BASURA");
	}
}

void crear_archivo(char* archivo){
	//TODO
}

void informar_falta_archivo(t_tarea* tarea, char* archivo){

	//ACA HAY QUE AVISARLE AL TRIPULANTE A CARGO DE LA TAREA Y FINALIZAR TAREA. TIENE QUE ESPERAR EN COLA DE BLOQUEADO.
	log_error(logger, "El archivo %s no existe. Finalizando tarea %s", archivo, tarea->tarea);

}
