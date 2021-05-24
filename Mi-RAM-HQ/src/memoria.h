#ifndef MI_RAM_HQ_SRC_MEMORIA_H_
#define MI_RAM_HQ_SRC_MEMORIA_H_

#include "utils_ram.h"
#include "mapa.h"

pthread_t hiloReceiveMapa;

void iniciar_memoria(void);
void preparar_memoria(void);
void preparar_memoria_para_esquema_de_segmentacion(void);
void preparar_memoria_para_esquema_de_paginacion(void);

void* convertir(char* algoritmo_nombre);
void seleccionar_victima_LRU(void);
void seleccionar_victima_CLOCK(void);
bool hay_marcos_libres(void);
entradaTablaMarcos* buscar_entrada(void* marco);
entradaTablaMarcos* asignar_entrada_marco_libre(void);
bool ambos_bits_apagados(entradaTablaMarcos* entrada);
bool bit_uso(entradaTablaMarcos* entrada);
bool bit_modificado(entradaTablaMarcos* entrada);

int cantidad_de_marcos;
int cantidad_de_marcos_swap;

#endif /* MI_RAM_HQ_SRC_MEMORIA_H_ */
