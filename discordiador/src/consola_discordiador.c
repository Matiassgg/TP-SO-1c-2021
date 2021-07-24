#include "consola_discordiador.h"

void leer_consola() {
	char* leido = readline(">");
	while (!son_iguales(leido, "\0")) {
		procesar_mensajes_en_consola_discordiador(string_split(leido, " "));
		free(leido);
		leido = readline(">");
	}
	free(leido);
}


char* estado_string(char estado){
	switch(estado){
		case 'n':case 'N':
			return "NEW";
		case 'r':case 'R':
			return "READY";
		case 'e':case 'E':
			return "EXEC";
		case 'b':case 'B':
			return "BLOCK I/O";
	}
	return "ERROR";
}

char* de_listado_a_string(t_list* lista_tripulantes){
	char* string_listado = string_new();
	string_append(&string_listado, "--------------------------------------------------------------------\n");
		string_append_with_format(&string_listado, "Estado de la Nave: %s\n", temporal_get_string_time("%d/%m/%y %H:%M:%S"));
	for(int i=0;i<list_size(lista_tripulantes);i++){
		t_respuesta_listar_tripulante* tripulante = list_get(lista_tripulantes,i);
		string_append_with_format(&string_listado, "Tripulante: %i	Patota: %i	Status: %s\n", tripulante->id_tripulante, tripulante->id_patota, estado_string(tripulante->estado));
	}
	string_append(&string_listado, "--------------------------------------------------------------------");

	return string_listado;
}

void procesar_mensajes_en_consola_discordiador(char** palabras_del_mensaje) {

	if(son_iguales(palabras_del_mensaje[0] ,"ESTAS_ON")) {
		if(chequear_argumentos_del_mensaje(palabras_del_mensaje + 1, 1)) {
			log_warning(logger, "DISCORDIADOR :: Falta especificar el modulo");
			return;
		}

		uint32_t socket_conexion;
		log_info(logger, "DISCORDIADOR :: Preguntamos si esta on %s", palabras_del_mensaje[1]);

		t_paquete* paquete_a_enviar = crear_paquete(ESTA_ON);

		if (son_iguales(palabras_del_mensaje[1] ,"Mi-RAM-HQ"))
			socket_conexion = socket_Mi_RAM_HQ;
		else if(son_iguales(palabras_del_mensaje[1] ,"i-Mongo-Store"))
			socket_conexion = socket_Mongo_Store;
		else
			log_warning(logger, "DISCORDIADOR :: A quien te trataste de conectar? Cri cri, cri cri");

		enviar_paquete(paquete_a_enviar, socket_conexion);

		t_respuesta respuesta = recibir_respuesta(socket_conexion);

		if (respuesta == FAIL)
			log_warning(logger, "DISCORDIADOR :: No se recibio respuesta");
		else
			log_info(logger, "DISCORDIADOR :: El modulo %s esta ON", palabras_del_mensaje[1]);

		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"INICIAR_PATOTA")) {

		log_info(logger, "DISCORDIADOR :: Iniciaremos la patota %i", ++cantidad_patotas);

		t_patota* patota = de_consola_a_patota(palabras_del_mensaje);
		patota->id_patota = cantidad_patotas;

		// Enviar la creacion de la patota
		enviar_iniciar_patota(patota,socket_Mi_RAM_HQ);
		enviar_iniciar_patota(patota,socket_Mongo_Store);

		// Empezar a planificar la patota
		planificar_patota(patota);
//		list_add(patotas, patota); VER SI ES NECESARIO

		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"LISTAR_TRIPULANTES")) {
		// Envio mensaje a RAM y el se encarga de armar el listado de cada wachin
		t_paquete* paquete_a_enviar = crear_paquete(LISTAR_TRIPULANTES);
		int socket_con_RAM;
	    if((socket_con_RAM = crear_conexion(ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ)) == -1)
	    	log_error(logger, "DISCORDIADOR :: No me pude conectar a Mi-RAM-HQ");
	    else
			log_info(logger, "DISCORDIADOR :: Me pude conectar a Mi-RAM-HQ");
		enviar_paquete(paquete_a_enviar, socket_con_RAM);
		t_list* lista_tripulantes = recibir_listado_tripulantes(socket_con_RAM);
		char* string_listado = de_listado_a_string(lista_tripulantes);

		log_info(logger, "\n\n%s\n", string_listado);

		free(string_listado);
		list_destroy_and_destroy_elements(lista_tripulantes, free);
		liberar_conexion(&socket_con_RAM);
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"EXPULSAR_TRIPULANTE")) {

		uint32_t id_tripulante = atoi(palabras_del_mensaje[1]);

		log_info(logger, "DISCORDIADOR :: Finalizamos al tripulante %d", id_tripulante);

		// Mover a exit
		finalizar_tripulante_plani(id_tripulante);

		// Elimina el TCB y lo vuela del mapa
//		enviar_RAM_expulsar_tripulante(id_tripulante,socket_Mi_RAM_HQ);

		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"INICIAR_PLANIFICACION")) {
		iniciar_planificacion();

		log_info(logger, "DISCORDIADOR :: Se da inicio a la planificacion");

		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"PAUSAR_PLANIFICACION")) {
		log_info(logger, "DISCORDIADOR :: Se esta deteniendo la planificacion");

		pausar_planificacion();

		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"OBTENER_BITACORA")) {
		uint32_t id_tripulante = atoi(palabras_del_mensaje[1]);
		log_info(logger, "DISCORDIADOR :: Obtenemos la bitacora del tripulante %i", id_tripulante);

		// Consulta a i-Mongo-Store para obtener la bitacora
		enviar_Mongo_obtener_bitacora(id_tripulante, socket_Mongo_Store);

		char* bitacora = recibir_obtener_bitacora_respuesta(socket_Mongo_Store);

		log_info(logger, "\nBITACORA DEL TRIPULANTE %i:\n%s\n", id_tripulante, bitacora);

		free(bitacora);
		return;
	}
}

t_patota* de_consola_a_patota(char** palabras_del_mensaje){
	t_patota* patota = malloc(sizeof(t_patota));
	patota->posiciones = list_create();
	uint32_t cant_posiciones = 0;

	patota->cant_tripulantes = (uint32_t) atoi(palabras_del_mensaje[1]);
	patota->tam_path = string_length(palabras_del_mensaje[2]);
	patota->path_tareas = string_duplicate(palabras_del_mensaje[2]);

	for(; palabras_del_mensaje[3+cant_posiciones] != NULL; cant_posiciones++){
		t_posicion* pos_ini = malloc(sizeof(t_posicion));
		char** posicion = string_split(palabras_del_mensaje[3+cant_posiciones], "|");

		pos_ini->pos_x = (uint32_t) atoi(posicion[0]);
		pos_ini->pos_y = (uint32_t) atoi(posicion[1]);
		log_info(logger, "posicion %i,%i", pos_ini->pos_x,pos_ini->pos_y);

		list_add(patota->posiciones, pos_ini);

		string_iterate_lines(posicion, (void*) free);
	}

	if(cant_posiciones < patota->cant_tripulantes) {
		for(;cant_posiciones < patota->cant_tripulantes; cant_posiciones++){
			t_posicion* pos_ini = malloc(sizeof(t_posicion));
			pos_ini->pos_x = 0;
			pos_ini->pos_y = 0;
			list_add(patota->posiciones, pos_ini);
		}
	}
	return patota;
}
