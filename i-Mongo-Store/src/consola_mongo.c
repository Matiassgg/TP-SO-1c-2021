#include "consola_mongo.h"

void leer_consola() {
	char* leido = readline(">");

	while (!son_iguales(leido, "\0")) {
/*
		if (sonIguales(palabras_del_mensaje[0] ,"estasON")) {
			if (contar_elementos_array(palabras_del_mensaje) != 2) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			}
			else {
				uint32_t socketConexion;
				log_info(logger, "MONGO :: Preguntamos si esta on %s", palabras_del_mensaje[1]);

				if (sonIguales(palabras_del_mensaje[1] ,"Mi-RAM-HQ"))
					socketConexion = conectar(logger, ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ);
				else if(sonIguales(palabras_del_mensaje[1] ,"discordiador"))
					socketConexion = conectar(logger, ip_discordiador, puerto_discordiador);

				t_paquete* paquete_a_enviar = crear_paquete(ESTAON);


				t_buffer* buffer = serializar_paquete(paquete_a_enviar);

				send(socketConexion, buffer->stream, (size_t) buffer->size, 0);

				liberar_conexion(&socketConexion);

				eliminar_paquete(paquete_a_enviar);
				eliminar_buffer(buffer);
			}
		}*/

		procesar_mensajes_en_consola_mongo(string_split(leido, " "), 1);

/*
		if (!strcmp(palabras_del_mensaje[0], "crearRestaurante")) {
			if (contar_elementos_array(palabras_del_mensaje) != 8) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			}
			else {
				log_info(logger, "SINDICATO :: Creamos el restaurante %s", palabras_del_mensaje[1]);

				char* path = crear_path_restaurante(palabras_del_mensaje[1]);
				crear_archivo_info(path, palabras_del_mensaje);

				free(path);
			}
		}

		if (!strcmp(palabras_del_mensaje[0], "crearReceta")) {
			if (contar_elementos_array(palabras_del_mensaje) != 4) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			}
			else {
				log_info(logger, "SINDICATO :: Creamos la receta %s", palabras_del_mensaje[1]);

				char* path = crear_path_receta(palabras_del_mensaje[1]);
				crear_archivo_receta(path, palabras_del_mensaje);

				free(path);
			}
		}
		*/

		free(leido);
		leido = readline(">");
	}
	free(leido);
}

void procesar_mensajes_en_consola_mongo(char** palabras_del_mensaje, int cantidadArgumentos) {

	// Check de mensaje valido ?¿¿
	/*
	if(existe_en_array(msg_strings_mongo, palabras_del_mensaje[0])) {
		log_info(logger, "No existe el mensaje");
		return;
	}*/

	if(chequear_argumentos_del_mensaje(palabras_del_mensaje + 1, cantidadArgumentos)) {
		log_info(logger, "Cantidad incorrecta de argumentos");
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"ESTAS_ON")) {
		uint32_t socketConexion;
		log_info(logger, "MONGO :: Preguntamos si esta on %s", palabras_del_mensaje[1]);

		if (son_iguales(palabras_del_mensaje[1] ,"Mi-RAM-HQ"))
			socketConexion = conectar(ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ);
		else if(son_iguales(palabras_del_mensaje[1] ,"discordiador"))
			socketConexion = conectar(ip_discordiador, puerto_discordiador);

		t_paquete* paquete_a_enviar = crear_paquete(ESTA_ON);


		t_buffer* buffer = serializar_paquete(paquete_a_enviar);

		send(socketConexion, buffer->stream, (size_t) buffer->size, 0);

		liberar_conexion(&socketConexion);

		eliminar_paquete(paquete_a_enviar);
		eliminar_buffer(buffer);
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"OTRO")) {
		// NADA
	}

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"MAS CASOS")) {
		// NADA
	}

}


