#include "utils_discordiador.h"

void arrancar_servidor(void){
	pthread_t thread;
	uint32_t socket_servidor = iniciar_servidor(logger, ip_discordiador, puerto_escucha);
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

	//Procesar mensajes recibidos
	switch (cod_op) {
		t_posicion* posicion_sabotaje = malloc(sizeof(t_posicion));
		t_respuesta_listado_tripulantes* listado_recibido = malloc(sizeof(t_respuesta_listado_tripulantes));
		listado_recibido->tripulantes = list_create();

		case ESTA_ON:
			log_info(logger, "Estamos on");
			enviar_respuesta(OK,cliente_fd);
		break;

		case RESPUESTA_LISTAR_TRIPULANTES:
			log_info(logger, "NOS LLEGO EL LISTADO DE LOS TRIPULANTES");
			listado_recibido = deserializar_respuesta_listado_tripulantes(cliente_fd);

			if (listado_recibido->cantidad_tripulantes == 0)
				log_warning(logger, "DISCORDIADOR :: No pudimos obtener informacion de ninguno de los tripulantes de Mi-RAM");
			else {
				t_respuesta_listar_tripulante* respuesta = malloc(sizeof(t_respuesta_listar_tripulante));

				log_info(logger, "--------------------------------------------------------");
				log_info(logger, "DISCORDIADOR :: Estado de la nave: %s", temporal_get_string_time("%d/%m/%y %H:%M:%S"));
				log_info(logger, "--------------------------------------------------------");

				for(int i = 0; i < listado_recibido->cantidad_tripulantes; i++) {
					respuesta = (t_respuesta_listar_tripulante*) list_get(listado_recibido->tripulantes, i);
					log_info(
							logger,
							"Tripulante: %i\t Patota: %i\t Estado: %s",
							respuesta->id_tripulante,
							respuesta->id_patota,
							obtener_estado_segun_caracter(respuesta->estado)
						);
					free(respuesta);
					// POR AHORA SE PRUEBA CON TRIPULANTES HARDCODEADOS EN RAM NADA MAS
				}
				free(respuesta);
			}
			log_info(logger, "--------------------------------------------------------");
		break;

		case SABOTAJE:
			posicion_sabotaje = deserializar_posicion_sabotaje(cliente_fd);
			log_info(logger, "DISCORDIADOR :: Me llego SABOTAJE de MONGO STORE en la posicion %i|%i",
					posicion_sabotaje->pos_x,
					posicion_sabotaje->pos_y
					);
			log_info(logger, "El sabotaje tendra una duracion de %d", duracion_sabotaje);


			// REVISAR
			detener_tripulantes();
			planificar_tripulante_para_sabotaje();

			free(posicion_sabotaje);
			break;
	}

}

void detener_tripulantes(){

	for(int i=0; i<list_size(lista_exec); i++){
		pthread_mutex_lock(&mutex_cola_exec);
		p_tripulante* tripulante_plani = (p_tripulante*) list_get(lista_exec,i);
		pthread_mutex_unlock(&mutex_cola_exec);

		pthread_mutex_lock(&tripulante_plani->mutex_ejecucion);

		list_add(lista_bloq_Emergencia, tripulante_plani->tripulante);
	}
	for(int i=0; i<queue_size(cola_ready); i++){
		pthread_mutex_lock(&mutex_cola_ready);
		p_tripulante* tripulante_plani = (p_tripulante*) queue_pop(cola_ready);
		pthread_mutex_unlock(&mutex_cola_ready);

		list_add(lista_bloq_Emergencia, tripulante_plani->tripulante);
	}
	for(int i=0; i<queue_size(cola_ready); i++){
		pthread_mutex_lock(&mutex_cola_bloqueados_io);
		p_tripulante* tripulante_plani = (p_tripulante*) queue_pop(cola_bloq_E_S);
		pthread_mutex_unlock(&mutex_cola_bloqueados_io);

		list_add(lista_bloq_Emergencia, tripulante_plani->tripulante);
	}



}


void rafaga_cpu(uint32_t tiempo){
	sleep(retardo_ciclo_cpu * tiempo);
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
