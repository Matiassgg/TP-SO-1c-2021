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

		log_info(logger, "--------------------------------------------------------");
		log_info(logger, "DISCORDIADOR :: Estado de la nave: %s", temporal_get_string_time("%d/%m/%y %H:%M:%S"));
		log_info(logger, "--------------------------------------------------------");

		// Envio mensaje a RAM
		t_paquete* paquete_a_enviar = crear_paquete(LISTAR_TRIPULANTES);
		enviar_paquete(paquete_a_enviar, socket_Mi_RAM_HQ);

		// Obtengo el listado a partir de RAM
		t_list* lista_tripulantes_respuesta = list_create();
		obtener_listado_tripulantes(lista_tripulantes_respuesta);
		uint32_t cantidad_tripulantes = list_size(lista_tripulantes_respuesta);

		if (cantidad_tripulantes == 0)
			log_warning(logger, "DISCORDIADOR :: No pudimos obtener info. de los tripulante de Mi-RAM");
		else {
			log_info(logger, "DISCORDIADOR :: Recorremos a los tripulantes");
			t_respuesta_listar_tripulantes* respuesta = malloc(sizeof(t_respuesta_listar_tripulantes));

			for(int i = 0; i < cantidad_tripulantes; i++) {
				respuesta = (t_respuesta_listar_tripulantes*) list_get(lista_tripulantes_respuesta, i);
				log_info(
						logger,
						"Tripulante: %i\t Patota: %i\t Estado: %s",
						respuesta->id_tripulante,
						respuesta->id_patota,
						obtener_estado_segun_caracter(respuesta->estado)
					);
				// SON VARIOS TRIPULANTES, POR AHORA SE PRUEBA CON UNO HARDCODEADO NADA MAS
			}
			free(respuesta);
		}
		log_info(logger, "--------------------------------------------------------");

		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"EXPULSAR_TRIPULANTE")) {

//		Con este comando se busca finalizar un tripulante y avisarle a Mi-RAM HQ que este tripulante es
//		eyectado para que deje de mostrarlo en el mapa y en caso de que sea necesario elimine su segmento
//		de tareas. Recibirá como parámetro el id del tripulante.

		log_info(logger, "DISCORDIADOR :: Expulsamos al tripulante %d", palabras_del_mensaje[1]);

		t_tripulante* tripulante = malloc(sizeof(t_tripulante));

		enviar_RAM_expulsar_tripulante(tripulante,socket_Mi_RAM_HQ);

		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"INICIAR_PLANIFICACION")) {
		if(chequear_argumentos_del_mensaje(palabras_del_mensaje + 1, 0))
			log_warning(logger, "No metas la pata, aca no hacen faltan argumentos");

//		Con este comando se dará inicio a la planificación, la idea es que hasta este punto no hayan
//		movimientos entre las colas de planificación ni de los tripulantes. Este mensaje no contiene ningún
//		parámetro.

		iniciar_planificacion();

		log_info(logger, "DISCORDIADOR :: Se da inicio a la planificacion");

		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"PAUSAR_PLANIFICACION")) {

//		Este comando lo que busca es detener la planificación en cualquier momento. Este mensaje no
//		contiene ningún parámetro.

		log_info(logger, "DISCORDIADOR :: Se esta deteniendo la planificacion");

		pausar_planificacion();

		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"OBTENER_BITACORA")) {
//		Este comando obtendrá la bitácora del tripulante pasado por parámetro a través de una consulta a
//		i-Mongo-Store.

		log_info(logger, "DISCORDIADOR :: Obtenemos la bitacora del tripulante %s", palabras_del_mensaje[1]);

		// Consulta a i-Mongo-Store para obtener la bitacora

		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"SABOTAJE")) {
		log_info(logger, "DISCORDIADOR :: Nos llego SABOTAJE de i-Mongo-Store");

		// Este mensaje llega a discordiador cuando le llega la señal al mongo
		// Falta resolver como averiguar la posicion de sabotaje desde aca

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

char* obtener_estado_segun_caracter(char estado) {
	char* estado_string;
	switch(estado) {
		case 'N':
			estado_string = "NEW";
			break;
		case 'R':
			estado_string =  "READY";
			break;
		case 'B':
			estado_string =  "BLOCK I/O";
			break;
		case 'E':
			estado_string =  "EXEC";
			break;
		default:
			estado_string = "ERROR";
			break;
	}
	return estado_string;
}

// ESTO VA EN RAM
void obtener_listado_tripulantes(t_list* lista_tripulantes) {
	t_respuesta_listar_tripulantes* respuesta_prueba = malloc(sizeof(t_respuesta_listar_tripulantes));

	// TODO HARDCODEADO : SOLO ES PRUEBA : SE PIDE A MEMORIA
	respuesta_prueba->id_tripulante = 1;
	respuesta_prueba->id_patota= 1;
	respuesta_prueba->estado = 'N';
	list_add(lista_tripulantes, respuesta_prueba);
}


