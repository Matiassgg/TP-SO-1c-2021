#include "i-Mongo-Store.h"
#define ARCHIVO_CONFIG "../i-Mongo-Store.config"

int main(int argc, char* argv[]) {
	iniciar_mongo();
	pthread_create(&hiloReceive, NULL, (void*) arrancar_servidor, NULL);
	pthread_detach(hiloReceive);

	leer_consola();

    return EXIT_SUCCESS;
}

void iniciar_mongo(void) {
	leer_config();
	logger = iniciar_logger(archivo_log, "i-Mongo-Store.c");
    log_info(logger, "Ya obtuvimos la config del mongo\n");
}

void leer_config(void) {
	config = config_create(ARCHIVO_CONFIG);


	ip_Mongo_Store = config_get_string_value(config, "IP_I_MONGO_STORE");
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	ip_Mi_RAM_HQ = config_get_string_value(config, "IP_MI_RAM_HQ");
	puerto_Mi_RAM_HQ = config_get_string_value(config, "PUERTO_MI_RAM_HQ");
	ip_discordiador = config_get_string_value(config, "IP_DISCORDIADOR");
	puerto_discordiador = config_get_string_value(config, "PUERTO_DISCORDIADOR");
	archivo_log = config_get_string_value(config, "ARCHIVO_LOG");
}
