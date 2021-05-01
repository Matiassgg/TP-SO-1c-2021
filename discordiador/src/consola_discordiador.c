#include "consola_discordiador.h"

void leer_consola() {
	char* leido = readline(">");


	while (!son_iguales(leido, "\0")) {

		procesar_mensajes_en_consola_discordiador(string_split(leido, " "), 1);

		free(leido);
		leido = readline(">");
	}
	free(leido);
}

void procesar_mensajes_en_consola_discordiador(char** palabras_del_mensaje, int cantidadArgumentos) {

	// Check de mensaje valido ?¿¿
	/*
	if(existe_en_array(msg_strings_discordiador , palabras_del_mensaje[0])) {
		log_info(logger, "No existe el mensaje");
		return;
	}*/

	if(chequear_argumentos_del_mensaje(palabras_del_mensaje + 1, cantidadArgumentos)) {
		log_info(logger, "Cantidad incorrecta de argumentos");
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"ESTAS_ON")) {
		uint32_t socket_conexion;
		log_info(logger, "DISCORDIADOR :: Preguntamos si esta on %s", palabras_del_mensaje[1]);

		if (son_iguales(palabras_del_mensaje[1] ,"Mi-RAM-HQ"))
			socket_conexion = conectar(logger, ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ);
		else if(son_iguales(palabras_del_mensaje[1] ,"i-Mongo-Store"))
			socket_conexion = conectar(logger, ip_Mongo_Store, puerto_Mongo_Store);
		else
			log_warning(logger, "A quien te trataste de conectar? Cri cri, cri cri");

		t_paquete* paquete_a_enviar = crear_paquete(ESTA_ON);
		t_buffer* buffer = serializar_paquete(paquete_a_enviar);

		send(socket_conexion, buffer->stream, (size_t) buffer->size, 0);

		liberar_conexion(&socket_conexion);
		eliminar_paquete(paquete_a_enviar);
		eliminar_buffer(buffer);
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"INICIAR_PATOTA")) {
		/*
		 Iniciar patota
		Recibirá como parámetro la cantidad de tripulantes que tendrá la patota,
		un archivo de tareas que deberán ejecutar los tripulantes de la patota
		(se indica el formato en la siguiente sección)
		y una lista de posiciones iniciales de los tripulantes.
		Por defecto, si no se especifica la posición de la totalidad de los tripulantes,
		se asume que los restantes inician en 0|0.
		Ejemplo,
		INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4
		 */
		log_info(logger, "DISCORDIADOR :: Iniciaremos la patota %i", ++cantidad_patotas);

		iniciar_patota* patota = de_consola_a_patota(palabras_del_mensaje);
//		list_add(patotas,patota); VER SI ES NECESARIO

//		VER QUE HACER CON LA PATOTA
//		CREAR TRIPULANTES?
//		AVISAR A LOS MODULOS

	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"MAS CASOS")) {
		// NADA
	}

}


iniciar_patota* de_consola_a_patota(char** palabras_del_mensaje){
	iniciar_patota* patota = malloc(sizeof(iniciar_patota));
	patota->posiciones = list_create();
	uint32_t cant_posiciones=0;

	patota->cant_tripulantes = (uint32_t) atoi(palabras_del_mensaje[1]);
	patota->tam_path = string_length(palabras_del_mensaje[2]);
	patota->path_tareas = string_duplicate(palabras_del_mensaje[2]);

	for(cant_posiciones;palabras_del_mensaje[3+cant_posiciones];cant_posiciones++){
		posicion pos;
		char** posicion = string_split(palabras_del_mensaje[3+cant_posiciones], "|");
//		char** posicion = string_get_string_as_array(palabras_del_mensaje[3+cant_posiciones]); si podemos ponerlas como [posX,posY]
		pos.pos_x = (uint32_t) atoi(posicion[0]);
		pos.pos_y = (uint32_t) atoi(posicion[1]);
		list_add(patota->posiciones,pos);
		string_iterate_lines(posicion,free);
	}
	if(cant_posiciones < patota->cant_tripulantes)
		for(cant_posiciones;cant_posiciones < patota->cant_tripulantes;cant_posiciones++){
			posicion pos;
			pos.pos_x = 0;
			pos.pos_y = 0;
			list_add(patota->posiciones,pos);
		}

	return patota;
}
