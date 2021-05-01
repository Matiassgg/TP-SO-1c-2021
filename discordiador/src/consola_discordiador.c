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

	// Check de mensaje valido ?¿¿
	/*
	if(existe_en_array(msg_strings_discordiador , palabras_del_mensaje[0])) {
		log_info(logger, "No existe el mensaje");
		return;
	}*/

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"ESTAS_ON")) {
		if(chequear_argumentos_del_mensaje(palabras_del_mensaje + 1, 1)) {
			log_warning(logger, "Falta especificar que modulo, para ver si esta en ON");
			return;
		}

		uint32_t socket_conexion;
		log_info(logger, "DISCORDIADOR :: Preguntamos si esta on %s", palabras_del_mensaje[1]);

		t_paquete* paquete_a_enviar = crear_paquete(ESTA_ON);
		t_buffer* buffer = serializar_paquete(paquete_a_enviar);


		if (son_iguales(palabras_del_mensaje[1] ,"Mi-RAM-HQ"))
			send(socket_Mi_RAM_HQ, buffer->stream, (size_t) buffer->size, 0);
		else if(son_iguales(palabras_del_mensaje[1] ,"i-Mongo-Store"))
			send(socket_Mongo_Store, buffer->stream, (size_t) buffer->size, 0);
		else
			log_warning(logger, "A quien te trataste de conectar? Cri cri, cri cri");

		eliminar_paquete(paquete_a_enviar);
		eliminar_buffer(buffer);
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"INICIAR_PATOTA")) {

		// Se chequea que por lo menos se pasen como argumentos :
			// La cantidad de tripulantes
			// Archivo de tareas
//		if(chequear_argumentos_del_mensaje(palabras_del_mensaje + 1, 2)) {
//			log_warning(logger, "Se necesita como minimo la cantidad de tripulantes y el path de tareas");
//			return;
//		} ????

		/*
			Iniciar patota
			Recibirá como parámetro la cantidad de tripulantes que tendrá la patota,
			un archivo de tareas que deberán ejecutar los tripulantes de la patota
			(se indica el formato en la siguiente sección)
			y una lista de posiciones iniciales de los tripulantes.
			Por defecto, si no se especifica la posición de la totalidad de los tripulantes,
			se asume que los restantes inician en 0|0.

		Ejemplo:
			INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4
		 */
		log_info(logger, "DISCORDIADOR :: Iniciaremos la patota %i", ++cantidad_patotas);

		t_iniciar_patota* patota = de_consola_a_patota(palabras_del_mensaje);
		patota->id_patota = cantidad_patotas;

		enviar_iniciar_patota(patota,socket_Mi_RAM_HQ);
		enviar_iniciar_patota(patota,socket_Mongo_Store);
//		list_add(patotas, patota); VER SI ES NECESARIO

//		VER QUE HACER CON LA PATOTA
//		CREAR TRIPULANTES?
//		AVISAR A LOS MODULOS

	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"LISTAR_TRIPULANTES")) {

		// No hace falta cortar la funcion aca o ni hace falta el cheque nose
		if(chequear_argumentos_del_mensaje(palabras_del_mensaje + 1, 0))
			log_warning(logger, "No metas la pata, aca no hacen faltan argumentos");

		/*
			Este mensaje devolverá el listado de tripulantes con su estado actual
			y la patota a la que pertenecen
		*/

		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		log_info(logger, "DISCORDIADOR :: Estado de la nave: %d/%02d-%02d %02d:%02d:%02d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
		t_list* tripulantes = de_consola_a_listado_tripulantes();


	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"EXPULSAR_TRIPULANTE")) {
		// NADA

//		Con este comando se busca finalizar un tripulante y avisarle a Mi-RAM HQ que este tripulante es
//		eyectado para que deje de mostrarlo en el mapa y en caso de que sea necesario elimine su segmento
//		de tareas. Recibirá como parámetro el id del tripulante.

	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"INICIAR_PLANIFICACION")) {
		// NADA
		if(chequear_argumentos_del_mensaje(palabras_del_mensaje + 1, 0))
			log_warning(logger, "No metas la pata, aca no hacen faltan argumentos");

//		Con este comando se dará inicio a la planificación, la idea es que hasta este punto no hayan
//		movimientos entre las colas de planificación ni de los tripulantes. Este mensaje no contiene ningún
//		parámetro.

	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"PAUSAR_PLANIFICACION")) {
		// NADA
		if(chequear_argumentos_del_mensaje(palabras_del_mensaje + 1, 0))
			log_warning(logger, "No metas la pata, aca no hacen faltan argumentos");

//		Este comando lo que busca es detener la planificación en cualquier momento. Este mensaje no
//		contiene ningún parámetro.

	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"OBTENER_BITACORA")) {
		// NADA

//		Este comando obtendrá la bitácora del tripulante pasado por parámetro a través de una consulta a
//		i-Mongo-Store.

	}
}

t_iniciar_patota* de_consola_a_patota(char** palabras_del_mensaje){
	t_iniciar_patota* patota = malloc(sizeof(t_iniciar_patota));
	patota->posiciones = list_create();
	uint32_t cant_posiciones = 0;

	patota->cant_tripulantes = (uint32_t) atoi(palabras_del_mensaje[1]);
	patota->tam_path = string_length(palabras_del_mensaje[2]);
	patota->path_tareas = string_duplicate(palabras_del_mensaje[2]);

	t_posicion* pos_ini = malloc(sizeof(t_posicion));
	for(; palabras_del_mensaje[3+cant_posiciones] != NULL; cant_posiciones++){
		char** posicion = string_split(palabras_del_mensaje[3+cant_posiciones], "|");
//		char** posicion = string_get_string_as_array(palabras_del_mensaje[3+cant_posiciones]); si podemos ponerlas como [posX,posY]

		pos_ini->pos_x = (uint32_t) atoi(posicion[0]);
		pos_ini->pos_y = (uint32_t) atoi(posicion[1]);
		list_add(patota->posiciones, pos_ini);

		string_iterate_lines(posicion, (void*) free);
	}

	if(cant_posiciones < patota->cant_tripulantes) {
		for(;cant_posiciones < patota->cant_tripulantes; cant_posiciones++){
			pos_ini->pos_x = 0;
			pos_ini->pos_y = 0;
			list_add(patota->posiciones, pos_ini);
		}
	}
	return patota;
}

t_list* de_consola_a_listado_tripulantes() {
	t_list* tripulantes = list_create();



	return tripulantes;
}
