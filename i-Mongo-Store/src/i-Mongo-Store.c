#include "i-Mongo-Store.h"


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
    FS_RESET();
    crear_punto_de_montaje();
}

void FS_RESET(){
	uint32_t borrarFS;
	printf("Desea reiniciar el FileSystem? (SI=1 | NO=0) :");
	scanf("%d",&borrarFS);

	if(borrarFS){
		char* aux = string_new();
		string_append_with_format(&aux, "sh eliminadorFS.sh %s", punto_montaje);
		system(aux);
		free(aux);
	}
}

void crear_punto_de_montaje(void){

}

void leer_config(void) {
	config = config_create(ARCHIVO_CONFIG);

	punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE");
	archivo_log = config_get_string_value(config, "PATH_ARCHIVO_LOG");
}

