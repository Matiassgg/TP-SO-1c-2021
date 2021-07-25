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
    int socket = *socket_que_nos_pasan;
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
		case ESTA_ON:
			log_info(logger, "Estamos on");
			enviar_respuesta(OK,cliente_fd);
		break;

//		case RESPUESTA_LISTAR_TRIPULANTES:
//			log_info(logger, "NOS LLEGO EL LISTADO DE LOS TRIPULANTES");
//			listado_recibido = (t_respuesta_listado_tripulantes*) deserializar_respuesta_listado_tripulantes(cliente_fd);
//
//			if (listado_recibido->cantidad_tripulantes == 0)
//				log_warning(logger, "DISCORDIADOR :: No pudimos obtener informacion de ninguno de los tripulantes de Mi-RAM");
//			else {
//				t_respuesta_listar_tripulante* respuesta = malloc(sizeof(t_respuesta_listar_tripulante));
//
//				log_info(logger, "--------------------------------------------------------");
//				log_info(logger, "DISCORDIADOR :: Estado de la nave: %s", temporal_get_string_time("%d/%m/%y %H:%M:%S"));
//				log_info(logger, "--------------------------------------------------------");
//
//				for(int i = 0; i < listado_recibido->cantidad_tripulantes; i++) {
//					respuesta = (t_respuesta_listar_tripulante*) list_get(listado_recibido->tripulantes, i);
//					log_info(
//							logger,
//							"Tripulante: %i\t Patota: %i\t Estado: %s",
//							respuesta->id_tripulante,
//							respuesta->id_patota,
//							obtener_estado_segun_caracter(respuesta->estado)
//						);
//					free(respuesta);
//					// POR AHORA SE PRUEBA CON TRIPULANTES HARDCODEADOS EN RAM NADA MAS
//				}
//				free(respuesta);
//			}
//			log_info(logger, "--------------------------------------------------------");
//		break;

		case SABOTAJE:
			posicion_sabotaje = deserializar_posicion_sabotaje(cliente_fd);
			log_info(logger, "DISCORDIADOR :: Me llego SABOTAJE de MONGO STORE en la posicion %i|%i",
					posicion_sabotaje->pos_x,
					posicion_sabotaje->pos_y
					);
			log_info(logger, "El sabotaje tendra una duracion de %d", duracion_sabotaje);

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

		log_info(logger, "Detenemos al tripulante %d en EXEC", tripulante_plani->tripulante->id);
		pthread_mutex_lock(&tripulante_plani->mutex_ejecucion);

		list_add(lista_bloq_Emergencia, tripulante_plani->tripulante);
	}
	for(int i=0; i<queue_size(cola_ready); i++){
		pthread_mutex_lock(&mutex_cola_ready);
		p_tripulante* tripulante_plani = (p_tripulante*) queue_pop(cola_ready);
		pthread_mutex_unlock(&mutex_cola_ready);

		log_info(logger, "Detenemos al tripulante %d en READY", tripulante_plani->tripulante->id);
		list_add(lista_bloq_Emergencia, tripulante_plani->tripulante);
	}
	for(int i=0; i<queue_size(cola_bloq_E_S); i++){
		pthread_mutex_lock(&mutex_cola_bloqueados_io);
		p_tripulante* tripulante_plani = (p_tripulante*) queue_pop(cola_bloq_E_S);
		pthread_mutex_unlock(&mutex_cola_bloqueados_io);

		log_info(logger, "Detenemos al tripulante %d en BLOQ I/O", tripulante_plani->tripulante->id);
		list_add(lista_bloq_Emergencia, tripulante_plani->tripulante);
	}

	log_info(logger,
			"Detuvimos a los tripulantes durante el sabotaje en la posicion %d|%d",
			posicion_sabotaje->pos_x,
			posicion_sabotaje->pos_y
			);
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
int conectar_con_RAM(){
	int socket_con_RAM;
	if((socket_con_RAM = crear_conexion(ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ)) == -1)
		log_error(logger, "DISCORDIADOR :: No me pude conectar a Mi-RAM-HQ");
	else
		log_info(logger, "DISCORDIADOR :: Me pude conectar a Mi-RAM-HQ");

	return socket_con_RAM;
}

int conectar_con_MONGO(){
	int socket_con_MONGO;
	if((socket_con_MONGO = crear_conexion(ip_Mongo_Store, puerto_Mongo_Store)) == -1)
		log_error(logger, "DISCORDIADOR :: No me pude conectar a i-Mongo-Store");
	else
		log_info(logger, "DISCORDIADOR :: Me pude conectar a i-Mongo-Store");

	return socket_con_MONGO;
}

// ELIJO A LA POSICION_A -> true
// ELIJO A LA POSICION_B -> false
bool posicion_mas_cercana(t_posicion* posicion_A, t_posicion* posicion_B, t_posicion* posicion_destino){
	bool resultado = true;
	int distancia_entre_puntos(int punto_A, int punto_B){
		return abs(punto_A-punto_B);
	}

	int distancia_x_A_D = distancia_entre_puntos(posicion_destino->pos_x, posicion_A->pos_x);
	int distancia_x_B_D = distancia_entre_puntos(posicion_destino->pos_x, posicion_B->pos_x);
	int distancia_y_A_D = distancia_entre_puntos(posicion_A->pos_y, posicion_destino->pos_y);
	int distancia_y_B_D = distancia_entre_puntos(posicion_B->pos_y, posicion_destino->pos_y);

	// La posicon A en x ES MEJOR que la posicion B en x
	if(distancia_x_A_D < distancia_x_B_D){

		// La posicon A en y ES MEJOR que la posicion B en y
		if(distancia_y_A_D < distancia_y_B_D){
			resultado = true;
		}

		// La posicon B en y ES MEJOR que la posicion A en y
		else if(distancia_y_A_D > distancia_y_B_D){

			// DESEMPATO POR CUAL ES MAS CHICA, si la posicion A en x o la posicion B en y
			if(distancia_x_A_D < distancia_y_B_D) {
				resultado = true;
			}

			else {
				resultado = false;
			}
		}

		else {
			resultado = true;
		}
	}

	// La posicon B en x ES MEJOR que la posicion A en x
	else if(distancia_x_A_D > distancia_x_B_D){

		// La posicon B en y ES MEJOR que la posicion A en y
		if(distancia_y_A_D > distancia_y_B_D){
			resultado = false;
		}

		// La posicon A en y ES MEJOR que la posicion B en y
		else if(distancia_y_A_D < distancia_y_B_D){

			// DESEMPATO POR CUAL ES MAS CHICA, si la posicion A en y o la posicion B en x
			if(distancia_x_A_D < distancia_y_B_D) {
				resultado = true;
			}

			else {
				resultado = false;
			}
		}

		else {
			resultado = true;
		}
	}

	// La posicon B en x ES IGUAL que la posicion A en x
	else {
		// DESEMPATO POR CUAL ES MAS CHICA, si la posicion A en y o la posicion B en y
		if(distancia_y_A_D < distancia_y_B_D){
			resultado = true;
		}
		else {
			resultado = false;
		}
	}

	return resultado;
}

void planificar_tripulante_para_sabotaje(){

	bool tripulante_en_exec_o_ready(p_tripulante* tripulante) {
		char estado = tripulante->tripulante->estado;
		return estado == 'E' || estado == 'R';
	}

	bool ordenar_segun_posicion(p_tripulante* tripulante_A, p_tripulante* tripulante_B) {
		return posicion_mas_cercana(tripulante_A->tripulante->posicion, tripulante_B->tripulante->posicion, posicion_sabotaje);
	}

	list_sort(lista_bloq_Emergencia, (void*) ordenar_segun_posicion);
	t_list* tripulantes = list_filter(lista_bloq_Emergencia, (void*) tripulante_en_exec_o_ready);
	t_tripulante* tripulante_mas_cercano = list_get(tripulantes, 0);

	//subir_tripulante_exec(tripulante_mas_cercano);
	log_info(logger, "POSICION SABOTAJE: %d|%d", posicion_sabotaje->pos_x, posicion_sabotaje->pos_y);
	log_info(logger, "ID TRIPULANTE MAS CERCANO: %d", tripulante_mas_cercano->id);
	log_info(logger, "POSICION TRIPULANTE: %d|%d", tripulante_mas_cercano->posicion->pos_x, tripulante_mas_cercano->posicion->pos_y);
}
