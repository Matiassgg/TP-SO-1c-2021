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
	logger = iniciar_logger_RAM(archivo_log, "Mi-RAM-HQ.c");
    log_info(logger, "Ya obtuvimos la config de Mi RAM HQ\n");

	//dic_discordiador_tabla_segmentos = dictionary_create();// REVISAR ESTO
	tablaDeMarcos = list_create();
	entradas_swap = list_create();
	patotas_creadas = list_create();
	tareas = list_create();

	pthread_mutex_init(&mutexTablaMarcos, NULL);
	pthread_mutex_init(&mutexEntradasSwap, NULL);
	pthread_mutex_init(&mutexMemoria, NULL);
	pthread_mutex_init(&mutexSwap, NULL);
	pthread_mutex_init(&mutexBuscarSegmento, NULL);
	pthread_mutex_init(&mutexBuscarPagina, NULL);
	pthread_mutex_init(&mutexBuscarInfoTripulante, NULL);
	pthread_mutex_init(&mutex_tocar_memoria, NULL);

    iniciar_memoria();

}

t_log* iniciar_logger_RAM(char* archivo_log, char* archivo_c) {
	return log_create(archivo_log, archivo_c, 0, LOG_LEVEL_INFO);
}

void leer_config() {
	config = config_create(ARCHIVO_CONFIG);

	pthread_mutex_init(&mutexTablaSegmentos, NULL);
	pthread_mutex_init(&mutexInfoPatotaAEnviar, NULL);
	pthread_mutex_init(&mutexEscribirMemoria, NULL);
	pthread_mutex_init(&mutexEliminarDeMemoria, NULL);
	pthread_mutex_init(&mutexPatotasCreadas, NULL);
	pthread_mutex_init(&mutexFree, NULL);
	pthread_mutex_init(&mutexVictima, NULL);

	archivo_log = config_get_string_value(config, "PATH_ARCHIVO_LOG");
	ip_Mi_RAM_HQ = config_get_string_value(config, "IP_RAM");
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");
	esquema_memoria = config_get_string_value(config, "ESQUEMA_MEMORIA");
	if(son_iguales(esquema_memoria,"PAGINACION")){
		tamanio_pagina = config_get_int_value(config, "TAMANIO_PAGINA");
		tamanio_swap = config_get_int_value(config, "TAMANIO_SWAP");
		path_swap = config_get_string_value(config, "PATH_SWAP");
		algoritmo_reemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	}
	else{
		criterio_seleccion = config_get_string_value(config, "CRITERIO_SELECCION");
	}
}

void finalizar_mi_ram() {

	log_destroy(logger);

	//	free(punto_montaje);
	free(memoria);
	config_destroy(config);

}
