#ifndef MI_RAM_HQ_SRC_MEMORIA_H_
#define MI_RAM_HQ_SRC_MEMORIA_H_

#include "utils_ram.h"
#include "mapa.h"

pthread_t hiloReceiveMapa;

void iniciar_memoria(void);
void preparar_memoria(void);
t_pcb* crear_pcb(t_patota* patota);
t_tcb* crear_tcbs(t_tripulante* tripulante);
void preparar_memoria_para_esquema_de_segmentacion(void);
void preparar_memoria_para_esquema_de_paginacion(void);

uint32_t escribir_en_memoria(void* informacion, e_tipo_dato tipo_dato);
uint32_t escribir_en_memoria_segmentacion(t_buffer* buffer);
t_segmento* dar_nuevo_segmento(t_segmento* segmento, uint32_t size);
void subir_segmento(t_segmento* segmento, void* stream);
t_segmento* buscar_segmento_libre(uint32_t espacio_requerido);

void escribir_en_memoria_paginacion(t_pcb* tcb, bool esta_en_memoria, uint32_t idPedido, bool modificado);

void* convertir(char* algoritmo_nombre);
void seleccionar_victima_LRU(void);
void seleccionar_victima_CLOCK(void);
bool hay_marcos_libres(void);
entradaTablaMarcos* buscar_entrada(void* marco);
entradaTablaMarcos* asignar_entrada_marco_libre(void);
bool ambos_bits_apagados(entradaTablaMarcos* entrada);
bool bit_uso_apagado(entradaTablaMarcos* entrada);
bool bit_uso(entradaTablaMarcos* entrada);
bool bit_modificado(entradaTablaMarcos* entrada);
int indice_elemento(t_list* lista, void* elemento);
int cantidad_de_marcos;
int cantidad_de_marcos_swap;

#endif /* MI_RAM_HQ_SRC_MEMORIA_H_ */
