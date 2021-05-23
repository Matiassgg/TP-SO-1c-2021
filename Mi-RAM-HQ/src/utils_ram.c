#include "utils_ram.h"

void arrancar_servidor(void){
	pthread_t thread;
	uint32_t socket_servidor = iniciar_servidor(logger, ip_Mi_RAM_HQ, puerto_escucha);
	while(1){
		uint32_t socket_cliente = esperar_cliente(logger,socket_servidor);

		int *socket_paraPasar = malloc(sizeof(int));
		*socket_paraPasar = socket_cliente;

		pthread_create(&thread, NULL, (void*) serve_client, socket_paraPasar);
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
	t_tripulante* tripulante = malloc(sizeof(t_tripulante));
	recv(cliente_fd, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	// logguear quien se me conecto: quiza hay que agregarle a los paquetes el nombre del modulo que envió el paquete, no lo sé

	//Procesar mensajes recibidos
	switch (cod_op) {
		case ESTA_ON:
			log_info(logger, "Estamos on");
			enviar_respuesta(OK,cliente_fd);
		break;
		case INICIAR_PATOTA:
			;
			t_patota* patota = deserializar_iniciar_patota(cliente_fd);

			log_info(logger, "RAM :: Nos llego INICIAR_PATOTA de la patota %i", patota->id_patota);

//			GUARDAR EN MEMORIA Y HACER LAS TARES CORRESPONDIENTES
//			POR AHORA SE HACE FREE CAPAZ DESPUES NO
			list_destroy(patota->posiciones);
			free(patota->path_tareas);
			free(patota);
		break;
		case INICIAR_TRIPULANTE:
			;
			tripulante = deserializar_iniciar_tripulante(cliente_fd);

			log_info(logger, "RAM :: Nos llego INICIAR_TRIPULANTE del tripulante %i", tripulante->id);

			crear_tripulante(nivel, tripulante);	// todo


//			GUARDAR EN MEMORIA Y HACER LAS TARES CORRESPONDIENTES
//			POR AHORA SE HACE FREE CAPAZ DESPUES NO
			free(tripulante);
			break;
		case LISTAR_TRIPULANTES:
			;
//			tripulante = deserializar_listar_tripulantes(cliente_fd);

			log_info(logger, "Nos llego LISTAR_TRIPULANTES");

			// Obtener el listado de todos los tripulantes con su estado y su patota correspondiente

//			enviar_discordiador_listar_tripulantes_respuesta(tripulantes, cliente_fd); es necesario????

//			log_info(logger, "RAM :: Se enviaron los datos de los tripulantes");

		break;
		case EXPULSAR_TRIPULANTE:
			;
			tripulante = deserializar_expulsar_tripulante(cliente_fd);

			log_info(logger, "RAM :: Nos llego EXPULSAR_TRIPULANTE para el tripulante %i", tripulante->id);

			// Eiminará un tripulante tanto de las estructuras administrativas de la memoria, como también del mapa
			// EN CASO QUE SEA NECESARIO eliminara su segmento de tareas

			// item_borrar(nivel, tripulante->id);			// todo borrar del mapa


			free(tripulante);	// por ahora no hay mucho que hacer uwu

			log_info(logger, "RAM :: Se expulso al tripulante");

		break;


	}

}
