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
