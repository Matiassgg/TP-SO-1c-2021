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
		t_list* lista_tripulantes_respuesta = list_create();
		t_posicion* posicion_sabotaje = malloc(sizeof(t_posicion));

		case ESTA_ON:
			log_info(logger, "Estamos on");
			enviar_respuesta(OK,cliente_fd);
		break;

		case LISTAR_TRIPULANTES:
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
		break;

		case SABOTAJE:
			posicion_sabotaje = deserializar_posicion_sabotaje(cliente_fd);
			log_info(logger, "DISCORDIADOR :: Me llego SABOTAJE de MONGO STORE en la posicion %i|%i",
					posicion_sabotaje->pos_x,
					posicion_sabotaje->pos_y
					);
			log_info(logger, "El sabotaje tendra una duracion de %d", duracion_sabotaje);


			// REVISAR
//			detener_tripulantes_con_tareas();
//			planificar_tripulante_para_sabotaje();

			free(posicion_sabotaje);
			break;
	}

}

void rafaga_cpu(uint32_t tiempo){
	sleep(retardo_ciclo_cpu * tiempo);
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
