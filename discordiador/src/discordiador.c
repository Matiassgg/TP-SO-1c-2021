#include "discordiador.h"

int main(int argc, char* argv[]) {
	iniciar_discordiador();
	pthread_create(&hiloReceive, NULL, (void*) arrancar_servidor, NULL);
	pthread_detach(hiloReceive);

	leer_consola();

	finalizar_discordiador();

    return EXIT_SUCCESS;
}

void iniciar_discordiador() {
	iniciar_patotas();
	leer_config();
	logger = iniciar_logger(archivo_log, "discordiador.c");
    log_info(logger, "Ya obtuvimos la config de discordiador\n");


	socket_Mi_RAM_HQ = conectar(logger, ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ);
	socket_Mongo_Store = conectar(logger, ip_Mongo_Store, puerto_Mongo_Store);

    log_info(logger, "Nos conectamos con los modulos\n");

    iniciar_planificacion();
}

void finalizar_discordiador(){
	liberar_conexion(&socket_Mi_RAM_HQ);
	liberar_conexion(&socket_Mongo_Store);

	queue_clean_and_destroy_elements(cola_new,free);
	queue_clean_and_destroy_elements(cola_ready,free);
	queue_clean_and_destroy_elements(cola_exec,free);
	queue_clean_and_destroy_elements(cola_bloq_E_S,free);
	queue_clean_and_destroy_elements(cola_bloq_Emergencia,free);
	queue_clean_and_destroy_elements(cola_exit,free);

	config_destroy(config);
//	free(punto_montaje);

	log_destroy(logger);
}

void iniciar_patotas(){
	cantidad_patotas = 0;
//	patotas = list_create(); VER SI ES NECESARIO
}

void leer_config() {
	config = config_create(ARCHIVO_CONFIG);

	ip_discordiador = config_get_string_value(config, "IP_DISCORDIADOR");
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	ip_Mi_RAM_HQ = config_get_string_value(config, "IP_MI_RAM_HQ");
	puerto_Mi_RAM_HQ = config_get_string_value(config, "PUERTO_MI_RAM_HQ");
	ip_Mongo_Store = config_get_string_value(config, "IP_I_MONGO_STORE");
	puerto_Mongo_Store = config_get_string_value(config, "PUERTO_I_MONGO_STORE");
	archivo_log = config_get_string_value(config, "PATH_ARCHIVO_LOG");
	algoritmo = config_get_string_value(config, "ALGORITMO");
	quantum = config_get_int_value(config, "QUANTUM");
}
