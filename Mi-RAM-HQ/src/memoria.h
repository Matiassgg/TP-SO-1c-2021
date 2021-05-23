#ifndef MI_RAM_HQ_SRC_MEMORIA_H_
#define MI_RAM_HQ_SRC_MEMORIA_H_

#include "utils_ram.h"
#include "mapa.h"

pthread_t hiloReceiveMapa;

void iniciar_memoria(void);
void preparar_memoria(void);
void preparar_memoria_para_esquema_de_segmentacion(void);
void preparar_memoria_para_esquema_de_paginacion(void);

#endif /* MI_RAM_HQ_SRC_MEMORIA_H_ */
