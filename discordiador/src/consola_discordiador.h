#ifndef DISCORDIADOR_SRC_CONSOLA_DISCORDIADOR_H_
#define DISCORDIADOR_SRC_CONSOLA_DISCORDIADOR_H_

#include "utils_discordiador.h"

void leer_consola();
void procesar_mensajes_en_consola_discordiador(char** palabras_del_mensaje);
t_iniciar_patota* de_consola_a_patota(char**);
t_list* de_consola_a_listado_tripulantes(void);

#endif /* DISCORDIADOR_SRC_CONSOLA_DISCORDIADOR_H_ */
