#include "consola_mongo.h"

t_posicion* obtener_proxima_posicion_sabotaje(void) {
	t_posicion* posicion = malloc(sizeof(t_posicion));

	// POR AHORA HARDCODEO :: Se obtiene de la lista
	posicion->pos_x = 10;
	posicion->pos_y = 10;

	//list_get(posiciones_sabotaje, proxima_posicion_sabotaje);

	return posicion;
}

void funcionFulera(int signal) {
	log_info(logger, "SE RECIBE EL SIGNAL %d", signal);
	t_posicion* posicion_sabotaje = obtener_proxima_posicion_sabotaje();
	enviar_discordiador_sabotaje(posicion_sabotaje, socket_discordiador);
}

// TODO HAY UNA CONSOLA EN MONGO? -> No es necesaria para mandar el signal o ke ?
void leer_consola() {
	signal(SIGUSR1, &funcionFulera);
	while (1) {
		// Tiene que seguir porque puede recibir mas señales de sabotaje supongo nose
	}
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


