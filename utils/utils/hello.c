#include "hello.h"

t_log* iniciar_logger(char* archivo_log, char* archivo_c) {
	return log_create(archivo_log, archivo_c, 1, LOG_LEVEL_INFO);
}

int contar_elementos_array(char** array) {
	int contador = 0;
	while (array[contador] != NULL) {
		contador++;
	}
	return contador;
}

int son_iguales(char* cadenaA, char* cadenaB) {
	return strcmp(cadenaA ,cadenaB) == 0;
}

int chequear_argumentos_del_mensaje(char** argumentos, int cantidad_argumentos_mensaje) {
	return contar_elementos_array(argumentos) != cantidad_argumentos_mensaje;
}

int existe_en_array(char** array, char* elemento_buscado) {
	int contador = 0;
	while (array[contador] != NULL || !son_iguales(elemento_buscado, array[contador])) {
		contador++;
	}
	return 1;
}

// El buen helloWorld, nada le gana
void helloWorld() {
    puts("Hello world with shared!\n!");
}
