#include "i-Mongo-Store.h"


int main(int argc, char* argv[]) {
	iniciar_mongo();
	pthread_create(&hiloReceive, NULL, (void*) arrancar_servidor, NULL);
	pthread_detach(hiloReceive);

	leer_consola();

    return EXIT_SUCCESS;
}

void iniciar_mongo(void) {
	logger = iniciar_logger(archivo_log, "i-Mongo-Store.c");
	leer_config();
    log_info(logger, "Ya obtuvimos la config del mongo");
    //FS_RESET();
   // crear_punto_de_montaje();

    inicializar_filesystem();

}

void inicializar_filesystem() {

    inicializar_paths_aux();
	if(!archivo_existe(ruta_superbloque))
		crear_superbloque();
	else
	    obtener_superbloque();

	if(!archivo_existe(path_bloques));
	    // inicializar_bloques();
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

void leer_config(void) {
	config = config_create(ARCHIVO_CONFIG);

	punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE");
	archivo_log = config_get_string_value(config, "PATH_ARCHIVO_LOG");
	tiempo_sincronizacion = config_get_int_value(config, "TIEMPO_SINCRONIZACION");
	blocks = config_get_int_value(config, "BLOCKS");
	block_size = config_get_int_value(config, "BLOCK_SIZE");

	posiciones_sabotaje = list_create();
	obtener_posiciones_sabotaje();
}

void obtener_posiciones_sabotaje() {
	char** posiciones_sabotaje_strings = config_get_array_value(config, "POSICIONES_SABOTAJE");
	for(uint32_t i = 0; posiciones_sabotaje_strings[i] != NULL; i++) {
		t_posicion* posicion = malloc(sizeof(t_posicion));
		char** posicion_string = string_split(posiciones_sabotaje_strings[i], "|");

		posicion->pos_x = (uint32_t) atoi(posicion_string[0]);
		posicion->pos_y = (uint32_t) atoi(posicion_string[1]);

		list_add(posiciones_sabotaje, posicion);
		string_iterate_lines(posicion_string, (void*) free);
	}
}

void terminar_programa() {

	log_info(logger,"\t\t\t~. MONGO STORE FINALIZADO .~\n");
	log_destroy(logger);

	config_destroy(config);

	//pthread_mutex_destroy(mutexBlocks);

	free(ruta_bloques);
	//free(bitarrayFS);
	free(ruta_superbloque);
//	free(mutexBlocks);
	free(path_files);

	//TODO Terminar conexiones...
}
