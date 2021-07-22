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

    conectar_modulos();
    preparar_planificacion();
    arrancar_planificacion();
}
void iniciar_patotas(){
	cantidad_patotas = 0;
	cantidad_tripulantes = 0;
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
	if(son_iguales(algoritmo, "RR"))
		quantum = config_get_int_value(config, "QUANTUM");
	grado_multitarea = config_get_int_value(config, "GRADO_MULTITAREA");
	duracion_sabotaje = config_get_int_value(config, "DURACION_SABOTAJE");
	retardo_ciclo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");
}

void conectar_modulos(){
    if((socket_Mi_RAM_HQ = crear_conexion(ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ)) == -1)
    	log_error(logger, "DISCORDIADOR :: No me pude conectar a Mi-RAM-HQ");
    else
		log_info(logger, "DISCORDIADOR :: Me pude conectar a Mi-RAM-HQ");
    if((socket_Mongo_Store = crear_conexion(ip_Mongo_Store, puerto_Mongo_Store)) == -1)
    	log_error(logger, "DISCORDIADOR :: No me pude conectar a i-Mongo-Store");
    else
		log_info(logger, "DISCORDIADOR :: Me pude conectar a i-Mongo-Store");

	if(socket_Mi_RAM_HQ == -1 || socket_Mongo_Store == -1)
		log_error(logger, "DISCORDIADOR :: No nos conectamos con ambos modulos\n");
	else if(socket_Mi_RAM_HQ != -1 && socket_Mongo_Store != -1)
		log_info(logger, "DISCORDIADOR :: Nos conectamos con ambos modulos\n");
}

void finalizar_discordiador(){
	log_info(logger,"\t\t\t~. Mi-RAM-HQ FINALIZADO .~\n");
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
