#include "Mi-RAM-HQ.h"

int main(int argc, char* argv[]) {
	iniciar_Mi_RAM_HQ();

	pthread_create(&hiloReceive, NULL, (void*) arrancar_servidor, NULL);
	pthread_join(hiloReceive, NULL);

	finalizar_mi_ram();

    return EXIT_SUCCESS;
}

void iniciar_Mi_RAM_HQ() {
	leer_config();
	logger = iniciar_logger(archivo_log, "Mi-RAM-HQ.c");
    log_info(logger, "Ya obtuvimos la config de Mi RAM HQ\n");
    iniciar_memoria();

}

void leer_config() {
	config = config_create(ARCHIVO_CONFIG);

	archivo_log = config_get_string_value(config, "PATH_ARCHIVO_LOG");
	ip_Mi_RAM_HQ = config_get_string_value(config, "IP_RAM");
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");
	esquema_memoria = config_get_string_value(config, "ESQUEMA_MEMORIA");
	tamanio_pagina = config_get_int_value(config, "TAMANIO_PAGINA");
	tamanio_swap = config_get_int_value(config, "TAMANIO_SWAP");
	path_swap = config_get_string_value(config, "PATH_SWAP");
	algoritmo_reemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
}

void finalizar_mi_ram() {

	log_destroy(logger);

	//	free(punto_montaje);
	free(memoria);
	config_destroy(config);

}
