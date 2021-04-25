#include "consola_mongo.h"

void leer_consola() {
	char* leido = readline(">");

	while (strcmp(leido, "\0")) {
		char** palabras_del_mensaje = string_split(leido, " ");

		if (!strcmp(palabras_del_mensaje[0], "estasON")) {
			if (contar_elementos_array(palabras_del_mensaje) != 2) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			}
			else {
				uint32_t socketConexion;
				log_info(logger, "MONGO :: Preguntamos si esta on %s", palabras_del_mensaje[1]);

				if (!strcmp(palabras_del_mensaje[1], "Mi-RAM-HQ"))
					socketConexion = conectar(logger, ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ);
				else if(!strcmp(palabras_del_mensaje[1], "discordiador"))
					socketConexion = conectar(logger, ip_discordiador, puerto_discordiador);

				t_paquete* paquete_a_enviar = crear_paquete(ESTAON);


				t_buffer* buffer = serializar_paquete(paquete_a_enviar);

				send(socketConexion, buffer->stream, &buffer->size, 0);

				liberar_conexion(&socketConexion);

				eliminar_paquete(paquete_a_enviar);
				eliminar_buffer(buffer);
			}
		}

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
		string_iterate_lines(palabras_del_mensaje,(void*) free);
		free(palabras_del_mensaje);
		leido = readline(">");
	}

	free(leido);
}
