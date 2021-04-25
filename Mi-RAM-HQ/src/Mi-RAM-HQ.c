#include "Mi-RAM-HQ.h"
#define ARCHIVO_CONFIG "../Mi-RAM-HQ.config"

int main(int argc, char* argv[]) {
	iniciar_Mi_RAM_HQ();
	pthread_create(&hiloReceive, NULL, (void*) arrancar_servidor, NULL);
	pthread_join(hiloReceive, NULL);
    return EXIT_SUCCESS;
}

void iniciar_Mi_RAM_HQ(void) {
	leer_config();
	logger = iniciar_logger(archivo_log, "Mi-RAM-HQ.c");
    log_info(logger, "Ya obtuvimos la config de Mi RAM HQ\n");
}

void leer_config(void) {
	config = config_create(ARCHIVO_CONFIG);

	ip_Mi_RAM_HQ = config_get_string_value(config, "IP_RAM");
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	archivo_log = config_get_string_value(config, "ARCHIVO_LOG");
}

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

void serve_client(int* socketQuenosPAsan) {
    log_info(logger, "entramos a serve_client");
	int socket = *socketQuenosPAsan;
	free(socketQuenosPAsan);

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
		case ESTAON:
			log_info(logger, "Estamos on");
		break;
	}

}
