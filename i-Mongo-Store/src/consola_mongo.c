#include "consola_mongo.h"

void procesar_nuevo_sabotaje(int signal) {
	if(hay_sabotajes()) {
		resolver_sabotaje();
		posicion_sabotaje = list_remove(posiciones_sabotaje, proxima_posicion_sabotaje);
		log_info(logger, "MANDAMOS LA POSICION DE SABOTAJE %d|%d", posicion_sabotaje->pos_x, posicion_sabotaje->pos_y);
	}
	else {
		log_warning(logger, "YA NO HAY MAS POSICIONES DE SABOTAJES");
		return;
	}

	// Levanto la conexion aca nomas, para los sabotajes ? Nose
    if((socket_discordiador = crear_conexion(ip_discordiador, puerto_discordiador)) == -1)
    	log_error(logger, "MONGO STORE :: No me pude conectar al DISCORDIADOR");
    else
    	log_info(logger, "MONGO STORE:: Pude conectar al DISCORDIADOR");

	enviar_discordiador_sabotaje(posicion_sabotaje, socket_discordiador);
	liberar_conexion(&socket_discordiador);
}

bool hay_sabotajes() {
	return proxima_posicion_sabotaje < list_size(posiciones_sabotaje);
}

void verificar_sabotajes() {
	signal(SIGUSR1, &procesar_nuevo_sabotaje);
}

// CREO QUE ESTO NO SE USA
void procesar_mensajes_en_consola_mongo(char** palabras_del_mensaje) {

	//////////////////////////////////////////////////////////////////////////////////////

	if(son_iguales(palabras_del_mensaje[0] ,"ESTAS_ON")) {
		uint32_t socket_conexion;
		log_info(logger, "MONGO :: Preguntamos si esta on %s", palabras_del_mensaje[1]);

		if (son_iguales(palabras_del_mensaje[1] ,"Mi-RAM-HQ"))
			socket_conexion = crear_conexion(ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ);
		else if(son_iguales(palabras_del_mensaje[1] ,"discordiador"))
			socket_conexion = crear_conexion(ip_discordiador, puerto_discordiador);

		t_paquete* paquete_a_enviar = crear_paquete(ESTA_ON);


		t_buffer* buffer = serializar_paquete(paquete_a_enviar);

		send(socket_conexion, buffer->stream, (size_t) buffer->size, 0);

		liberar_conexion(&socket_conexion);

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


